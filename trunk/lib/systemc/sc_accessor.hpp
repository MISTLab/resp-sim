/***************************************************************************\
 *
 *
 *         ___           ___           ___           ___
 *        /  /\         /  /\         /  /\         /  /\
 *       /  /::\       /  /:/_       /  /:/_       /  /::\
 *      /  /:/\:\     /  /:/ /\     /  /:/ /\     /  /:/\:\
 *     /  /:/~/:/    /  /:/ /:/_   /  /:/ /::\   /  /:/~/:/
 *    /__/:/ /:/___ /__/:/ /:/ /\ /__/:/ /:/\:\ /__/:/ /:/
 *    \  \:\/:::::/ \  \:\/:/ /:/ \  \:\/:/~/:/ \  \:\/:/
 *     \  \::/~~~~   \  \::/ /:/   \  \::/ /:/   \  \::/
 *      \  \:\        \  \:\/:/     \__\/ /:/     \  \:\
 *       \  \:\        \  \::/        /__/:/       \  \:\
 *        \__\/         \__\/         \__\/         \__\/
 *
 *
 *
 *
 *   This file is part of ReSP.
 *
 *   TRAP is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public License
 *   along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *   or see <http://www.gnu.org/licenses/>.
 *
 *
 *
 *   (c) Giovanni Beltrame, Luca Fossati
 *       Giovanni.Beltrame@esa.int fossati@elet.polimi.it
 *
\***************************************************************************/


/**
 * This class is a simple struct that allows access to SystemC classes
 * (a horrible hack)
 *
 */

 #include <systemc.h>

 namespace sc_core {

 struct sc_accessor {
    //-- Pointers to private stuff
    //
       enum execution_phases {
        phase_initialize = 0,
        phase_evaluate,
        phase_update,
        phase_notify
    };
    sc_object_manager*          m_object_manager;

    sc_module_registry*         m_module_registry;
    sc_port_registry*           m_port_registry;
    sc_export_registry*         m_export_registry;
    sc_prim_channel_registry*   m_prim_channel_registry;

    sc_name_gen*                m_name_gen;

    sc_process_table*           m_process_table;
    sc_curr_proc_info*          m_curr_proc_info;
    sc_object*                  m_current_writer;
    bool*                       m_write_check;
    int*                        m_next_proc_id;

    std::vector<sc_object*>*    m_child_objects;

    std::vector<sc_event*>*     m_delta_events;
    sc_ppq<sc_event_timed*>*    m_timed_events;

    std::vector<sc_trace_file*> m_trace_files;
    bool*                       m_something_to_trace;

    sc_runnable*                m_runnable;

    sc_time_params*             m_time_params;
    sc_time*                    m_curr_time;

    sc_dt::uint64*              m_delta_count;
    bool*                       m_forced_stop;
    bool*                       m_ready_to_simulate;
    bool*                       m_elaboration_done;
    execution_phases*           m_execution_phase;
    bool*                       m_error;
    bool*                       m_in_simulator_control;
    bool*                       m_end_of_simulation_called;
    bool*                       m_start_of_simulation_called;

    sc_event*                   m_until_event;

    sc_cor_pkg*                 m_cor_pkg; // the simcontext's coroutine package
    sc_cor*                     m_cor;     // the simcontext's coroutine

    inline
    sc_method_handle
    sc_simcontext::pop_runnable_method()
    {
        sc_method_handle method_h = m_runnable->pop_method();
        if( method_h == 0 ) {
        reset_curr_proc();
        return 0;
        }
        set_curr_proc( (sc_process_b*)method_h );
        return method_h;
    }

    inline
    sc_thread_handle
    sc_simcontext::pop_runnable_thread()
    {
        sc_thread_handle thread_h = m_runnable->pop_thread();
        if( thread_h == 0 ) {
            reset_curr_proc();
            return 0;
        }
        set_curr_proc( (sc_process_b*)thread_h );
        return thread_h;
    }


    /// Redefinable simulation function
    inline void simulate();

    void set_callback(void* cback); {
        delta_callback = cback;
    }

 };



 }
