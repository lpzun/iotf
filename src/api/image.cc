/**
 * image.cc
 *
 *  Created on: Dec 26, 2015
 *      Author: lpzun
 */

#include "image.hh"

namespace iotf {

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
void parser::parse(const string& filename, const mode& m) {
	if (m == mode::PREV) {
		this->parse_in_prev_mode(filename);
	} else if (m == mode::PREV) {
		this->parse_in_post_mode(filename);
	}
}

/**
 * @brief parse Boolean programs in  preimage mode
 * @param filename
 */
void parser::parse_in_prev_mode(const string& filename) {

}

/**
 * @brief parse Boolean programs in postimage mode
 * @param filename
 */
void parser::parse_in_post_mode(const string& filename) {

}

/**
 * @brief convert a system state (user-form global state) to a program state
 *        (our otf-form global state)
 * @param s
 * @param Z
 * @return a program state
 */
global_state converter::convert(const uint& s, const map<uint, uint>& Z) {
	// TODO
	return global_state();
}

/**
 * @brief convert a program state (our otf-form global state) to a system state
 *        (user-form global state)
 * @param gs
 * @return a pair
 */
pair<uint, map<uint, uint>> converter::convert(const global_state& gs) {
	// TODO
	return std::make_pair<uint, map<uint, uint>>(0, map<uint, uint>());
}

/**
 * @brief convert a shared system state to a shared program state
 * @param ss
 * @return
 */
vector<bool> converter::convert_ss_to_ps(const uint& ss) {
	vector<bool> sv(refs::SHARED_VARS_NUM, false);
	int pos = 0;
	while (pos < refs::SHARED_VARS_NUM) {
		if ((ss >> pos) & 1)
			sv[pos] = true;
	}
	return sv;
}

/**
 * @brief
 * @param ps
 * @return
 */
uint converter::convert_ps_to_ss(const vector<bool>& ps) {

}

} /* namespace otf */
