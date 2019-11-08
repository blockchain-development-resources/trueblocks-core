/*-------------------------------------------------------------------------------------------
 * qblocks - fast, easily-accessible, fully-decentralized data from blockchains
 * copyright (c) 2018, 2019 TrueBlocks, LLC (http://trueblocks.io)
 *
 * This program is free software: you may redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version. This program is
 * distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details. You should have received a copy of the GNU General
 * Public License along with this program. If not, see http://www.gnu.org/licenses/.
 *-------------------------------------------------------------------------------------------*/
#include "options.h"

//---------------------------------------------------------------------------------------------------
static const COption params[] = {
    // BEG_CODE_OPTIONS
    COption("freshen", "f", "", OPT_SWITCH, "Freshen price database (append new data)"),
    COption("period", "p", "enum[5|15|30|60|120*|240|1440]", OPT_FLAG, "display prices in this increment"),
    COption("pair", "a", "<string>", OPT_FLAG, "which price pair to freshen or list (see Poloniex)"),
    COption("", "", "", OPT_DESCRIPTION, "Freshen and/or display Ethereum price data."),
    // END_CODE_OPTIONS
};
static const size_t nParams = sizeof(params) / sizeof(COption);

//---------------------------------------------------------------------------------------------------
bool COptions::parseArguments(string_q& command) {

    if (!standardOptions(command))
        return false;

    // BEG_CODE_LOCAL_INIT
    string_q period = "";
    string_q pair = "";
    // END_CODE_LOCAL_INIT

    string_q format;
    Init();
    explode(arguments, command, ' ');
    for (auto arg : arguments) {
        string_q orig = arg;
        if (false) {
            // do nothing -- make auto code generation easier
            // BEG_CODE_AUTO
        } else if (arg == "-f" || arg == "--freshen") {
            freshen = true;

        } else if (startsWith(arg, "-p:") || startsWith(arg, "--period:")) {
            if (!confirmEnum("period", period, arg))
                return false;

        } else if (startsWith(arg, "-a:") || startsWith(arg, "--pair:")) {
            pair = substitute(substitute(arg, "-a:", ""), "--pair:", "");

        } else if (startsWith(arg, '-')) {  // do not collapse

            if (!builtInCmd(arg)) {
                return usage("Invalid option: " + arg);
            }

            // END_CODE_AUTO
        } else {
            return usage("Invalid option: " + arg);

        }
    }

    // Data wrangling
    if (!pair.empty())
        source.pair = pair;

    if (!period.empty())
        freq = str_2_Uint(period);

    string_q unused;
    establishFolder(source.getDatabasePath(unused));

    // Display formatting
    configureDisplay("ethQuote", "CAccountName", STR_DISPLAY_PRICEQUOTE);

    return true;
}

//---------------------------------------------------------------------------------------------------
void COptions::Init(void) {
    registerOptions(nParams, params);

    // BEG_CODE_INIT
    freshen = false;
    // END_CODE_INIT

    freq = 120;
    first = true;
    if (isApiMode())
        minArgs = 0;
}

//---------------------------------------------------------------------------------------------------
COptions::COptions(void) {
    setSorts(GETRUNTIME_CLASS(CBlock), GETRUNTIME_CLASS(CTransaction), GETRUNTIME_CLASS(CReceipt));
    Init();
    // BEG_CODE_NOTES
    notes2.push_back("Valid pairs include any pair from the public Poloniex's API here: | https://poloniex.com/public?command=returnCurrencies.");
    notes2.push_back("`Note`: Due to restrictions from Poloniex, this tool retrieves only 30 days of data | at a time. You must repeatedly run this command until the data is up-to-date.");
    // END_CODE_NOTES
}

//--------------------------------------------------------------------------------
COptions::~COptions(void) {
}
