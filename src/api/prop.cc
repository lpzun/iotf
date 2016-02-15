/**
 * @brief prop.cc: it serves as a supporting source file to store all other
 *        class declared in the header image.hh, like
 *        class parser and converter (the default implementations for virtual
 *        functions)
 *        Q: Why I named it as prop.cc?
 *        A: Because I think this file defines the common properties of the
 *        entire API. That's it!
 *
 * @date   Dec 26, 2015
 * @author Peizun Liu
 */

#include "../iotf.hh"

/// declare yyin to let parser read from file
/// NOTE: this is not an elegant way to do this
extern FILE * yyin;

namespace iotf {

cfg parser::prev_G; /// control flow graph in PREV mode
cfg parser::post_G; /// control flow graph in POST mode

/**
 * @brief default constructor
 */
parser::parser() {

}

/**
 * @brief default destructor
 */
parser::~parser() {

}

/**
 * @brief parse Boolean programs based on the mode (preimage or postimage)
 * @param filename: Boolean program
 * @param m       : mode
 */
pair<initl_ps, final_ps> parser::parse(const string& filename, const mode& m) {
    if (m == mode::PREV) {
        return parse_in_prev_mode(filename);
    } else if (m == mode::POST) {
        return parse_in_post_mode(filename);
    } else {
        throw iotf_runtime_error("there is no such mode!");
    }
}

/**
 * @brief parse Boolean programs in  preimage mode
 * @param filename
 */
pair<initl_ps, final_ps> parser::parse_in_prev_mode(const string& filename) {
    initl_ps I;
    final_ps Q;
    // TODO initialize prev_G
    return std::make_pair(I, Q);
}

/**
 * @brief parse Boolean programs in postimage mode
 * @param filename
 */
pair<initl_ps, final_ps> parser::parse_in_post_mode(const string& filename) {
    initl_ps I;
    final_ps Q;

    FILE *bPfile = fopen(filename.c_str(), "r");
    if (!bPfile) {
        throw iotf_runtime_error(filename + " open failed!");
    }
    yyin = bPfile;

    /// file list
    paide aide;
    yy::bp parser(aide); // make a parser
    int result = parser.parse(); // and run it
    if (result != 0) {
        throw iotf_runtime_error(
                "Parser exit with exception: " + std::to_string(result));
    }

    /* fw_aide aide; */
    //move the file point to the begin and print the total line number
    cout << "shared, local, line\n";
    refs::S_VARS_NUM = aide.s_vars_num;
    refs::L_VARS_NUM = aide.l_vars_num;
    cout << refs::S_VARS_NUM << ", " << refs::L_VARS_NUM << ", " << aide.lineno
            << "\n";
    aide.test_output_control_flow_graph();
    cout << endl;
    for (const auto& p : aide.s_vars_list)
        cout << p.first << " " << p.second << " " << aide.encode(p.first)
                << "\n";
    for (const auto& p : aide.l_vars_list)
        cout << p.first << " " << p.second << " " << aide.encode(p.first)
                << "\n";
    if (aide.is_failed)
        cout << "ooooooooooooooo\n";
    return std::make_pair(I, Q);
}

/**
 * @brief compute initial states
 * @param s_vars_init
 * @param l_vars_init
 * @return initial states
 */
initl_ps parser::create_initl_state(const map<ushort, sool>& s_vars_init,
        const map<ushort, sool>& l_vars_init) {
    initl_ps ps;
    return ps;
}

/**
 * @brief compute final states
 * @return final states
 */
final_ps parser::create_final_state() {
    final_ps fs;
    return fs;
}

///////////////////////////////////////////////////////////////////////////////
/// from here: all member definitions of class converter                    ///
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief convert a list of system states (user-form global state) to a list of
 *        program states (our otf-form global state)
 * @param ss: a list of system states
 * @return    a list of program states
 */
deque<prog_state> converter::convert(const deque<syst_state>& ss) {
    deque<prog_state> ps;
    for (const auto& s : ss)
        ps.emplace_back(this->convert(s));
    return ps;
}

/**
 * @brief convert a list of program states (user-form global state) to a list of
 *        system states (our otf-form global state)
 * @param ss: a list of program states
 * @return    a list of system states
 */
deque<syst_state> converter::convert(const deque<prog_state>& ps) {
    deque<syst_state> ss;
    for (const auto& p : ps)
        ss.emplace_back(this->convert(p));
    return ss;
}

/**
 * @brief convert a system state (user-form global state) to a program state
 *        (our otf-form global state)
 * @param s
 * @param Z
 * @return a program state
 */
prog_state converter::convert(const syst_state& ss) {
    // TODO
    return global_state();
}

/**
 * @brief convert a program state (our otf-form global state) to a system state
 *        (user-form global state)
 * @param gs
 * @return a pair
 */
syst_state converter::convert(const prog_state& ps) {
    // TODO
    return std::make_pair<uint, map<uint, uint>>(0, map<uint, uint>());
}

/**
 * @brief convert a shared system state to a shared program state
 * @param ss
 * @return a shared program state
 */
state_v converter::convert_sss_to_sps(const uint& ss) {
    return state_v(ss);
}

/**
 * @brief convert a shared program state to a shared system state
 * @param ps
 * @return a shared system state
 */
uint converter::convert_sps_to_sss(const state_v& ps) {
    return ps.to_ulong();
}

/**
 * @brief:
 *       low pc               local          high
 *           ________________ _______________
 *          |________________|_______________|
 *
 * @param ss
 * @return
 */
pair<size_pc, state_v> converter::convert_lss_to_lps(const uint& ss) {
    size_pc pc = ss & mask;
    state_v lv(ss >> SIZE_B / 2);
    return std::make_pair(pc, lv);
}

/**
 *
 * @param pc
 * @param ps
 * @return
 */
uint converter::convert_lps_to_lss(const size_pc& pc, const state_v& ps) {
    auto lv = (ps << (SIZE_B / 2)).to_ullong();
    return lv ^ pc;
}

} /* namespace otf */
