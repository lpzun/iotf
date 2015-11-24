/**
 * image.cc
 *
 *  Created on: Nov 16, 2015
 *      Author: lpzun
 */

#include "image.hh"

namespace iotf {

post_image::post_image() {

}

post_image::~post_image() {

}

/**
 * @brief implement the pure virtual function <step> from base case image
 * @param tau
 * @return cover successors
 *         a set of cover successors
 */
deque<global_state> post_image::step(const global_state& tau) {
	auto G = this->build_CFG("");
	return this->compute_cov_successors(tau, G);
}

void post_image::parser(const string& filename) {

}

cfg post_image::build_CFG(const string& filename) {
	cfg G;
	return G;
}

/**
 * @brief compute the cover successors of configuration tau
 * @param tau
 * @param G
 * @return
 */
deque<global_state> post_image::compute_cov_successors(const global_state& tau,
		const cfg& G) {
	deque<global_state> successors;
	const auto& sh = tau.get_s(); /// shared state
	const auto& Z = tau.get_locals();
	for (auto il = Z.cbegin(); il != Z.cend(); ++il) {
		const auto& local = il->first;
		const auto& pc = local.get_pc();    /// current pc
		const auto& lo = local.get_vars();  /// local vars
		for (auto ipc = G.get_A()[pc].cbegin(); ipc != G.get_A()[pc].cend();
				++ipc) {
			const auto& e = G.get_E()[*ipc]; /// get the edge point by pc
			const auto& _pc = e.get_dest();
			switch (e.get_stmt().get_type()) {
			case type_stmt::GOTO: {
				/// goto statement
				///   pc: goto <_pc>;
				/// pc+1: ...
				///
				/// SEMANTIC: nondeterministic goto
				local_state _local(_pc, lo);
				auto _Z = alg::update_counters(_local, local, Z);
				successors.emplace_back(sh, _Z);
			}
				break;
			case type_stmt::ASSG: {
				/// parallel statement
				///   pc: <id>+ := <expr>+ constrain <expr>;
				/// pc+1: ...
				///
				/// SEMANTIC: assignment statement, postcondition of
				/// vars might have to satisfy the constraint

			}
				break;
			case type_stmt::IFEL: {
				/// if ... else ... statement: conditional statement
				///   pc: if (<expr>) then <sseq> else <sseq>; fi;
				/// pc+1: ...
				///
				/// SEMANTIC:
				if (e.get_stmt().get_precondition().eval(sh.get_vars(), lo)) {
					// TODO
				} else {
					// TODO
				}
			}
				break;
			case type_stmt::ASSE: {
				/// assert statement
				///   pc: assert ( <expr> );
				/// pc+1: ...
				///
				/// SEMANTIC: assertion statement: encoding bad states

			}
				break;
			case type_stmt::ASSU: {
				/// assume statement: conditional statement
				///   pc: assume ( <expr> );
				/// pc+1: ...
				///
				/// SEMANTIC: advance if expr is evaluated to be true;
				/// block otherwise.
				if (e.get_stmt().get_precondition().eval(sh.get_vars(), lo)) {
					/// successor local state: l'.pc = l.pc + 1
					local_state _local(_pc, lo);
					auto _Z = alg::update_counters(_local, local, Z);
					successors.emplace_back(sh, _Z);
				}
			}
				break;
			case type_stmt::NTHR: {
				/// thread creation statement like:
				///   pc: start_thread <label>;
				/// pc+1: ...
				///
				/// SEMANTIC: the start_thread <label> instruction creates a new
				/// thread that starts execution at the program location label.
				/// It gets a copy of the local variables of the current thread,
				/// which continues execution at the proceeding statement.
				deque<local_state> T_in;
				/// create a new thread whose l''.pc = label and append it to T_in
				T_in.emplace_back(_pc, lo);
				/// update current thread that l'.pc = l.pc + 1 and append it T_in
				T_in.emplace_back(_pc + 1, lo);
				auto _Z = alg::update_counters(T_in, local, Z);
				successors.emplace_back(sh, _Z);
			}
				break;
			case type_stmt::ETHR: {
				/// thread termination statement
				///   pc: end_thread;
				/// pc+1: ...
				///
				/// SEMANTIC: the end_thread statement terminates the actual thread,
				/// i.e., has no successor state.
			}
				break;
			case type_stmt::ATOM: {
				/// the beginning statement of atomic section
				///   pc: atomic_begin;
				///   ...
				///  pc': atomic_end;
				/// SEMANTIC: the atomic_begin statement prevents the scheduler from
				/// a context switch to an other thread
				/// NOTE    : the atomic_end statement is not processed here, but in
				/// the subroutine.

				/// ns, i.e., new shared state, is to store the final shared state
				/// after across atomic section
				auto ns = sh;
				auto T_in = this->compute_image_atom_sect(ns, local);
				auto _Z = alg::update_counters(T_in, local, Z);
				successors.emplace_back(ns, _Z);
			}
				break;
			case type_stmt::BCST: {
				/// the broadcast statement
				///   pc: broadcast;
				/// pc+1: ...
				/// SEMANTIC: advance the pc of active thread, and wake up all waiting
				/// thread via advancing their pcs.
				///
				/// An example of broadcast:
				///      10: wait;
				///      11: ...
				///          ...
				///      20: wait;
				///      21: ...
				///          ...
				///      30: broadcast;
				/// 	 31: ...
				///          ...
				///
				///    (11, 21, 31, ...) <-
				///                         (10, 20, 30, ...)
				///                         (11, 20, 30, ...)
				///                         (10, 21, 30, ...)
				///                         (11, 21, 30, ...)
				auto _Z(Z);
				/// advance all blocking threads at the wait statements
				for (auto ilo = _Z.begin(); ilo != _Z.end(); ++ilo) {
					if (G.get_E()[ilo->first.get_pc()].get_stmt().get_type()
							== type_stmt::WAIT) {
						auto l_wait(ilo->first);
						l_wait.set_pc(ilo->first.get_pc() + 1);
						auto n_wait(ilo->second);
						_Z.erase(ilo); /// remove old pair
						_Z.emplace(l_wait, n_wait);
					}
				}

				/// advance the broadcast thread
				auto l_bcst(local);
				l_bcst.set_pc(local.get_pc() + 1);
				_Z = alg::update_counters(l_bcst, local, _Z);
				successors.emplace_back(sh, _Z);
			}
				break;
			case type_stmt::WAIT: {
				/// do nothing
			}
				break;
			default: {
				/// it mostly says the skip statement
				///   pc: skip;
				/// pc+1: ...
				/// SEMANTIC: advance the pc to pc + 1

				/// successor local state: l'.pc = l.pc + 1
				local_state _local(_pc, lo);
				auto _Z = alg::update_counters(_local, local, Z);
				successors.emplace_back(sh, _Z);
			}
				break;
			}
		}
	}
	return successors;
}

/**
 * @brief compute post images across an atomic section:
 *        Becareful that an atomic section could contain various statements.
 *
 * @param s: shared state at the beginning of atomic section
 *           It's also used to return the final shared state after the
 *           atomic section.
 * @param l: local state at the beginning of atomic section
 * @return succeeding local states after across atomic section
 */
deque<local_state> post_image::compute_image_atom_sect(shared_state& s,
		const local_state& l) {
	deque<local_state> T_in;
	return T_in;
}

} /* namespace otf */