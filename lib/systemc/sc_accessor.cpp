
#include <sc_accessor.hpp>

extern void* delta_callback(void);

void sc_accessor::set_callback(void* cback) {
    delta_callback = cback;
}


void sc_accessor:simulate(){

    while ( true ) {

	// EVALUATE PHASE
	
	m_execution_phase = phase_evaluate;
	while( true ) {


	    // execute method processes

	    sc_method_handle method_h = pop_runnable_method();
	    while( method_h != 0 ) {
		try {
		    method_h->semantics();
		}
		catch( const sc_report& ex ) {
		    ::std::cout << "\n" << ex.what() << ::std::endl;
		    m_error = true;
		    return;
		}
		method_h = pop_runnable_method();
	    }

	    // execute (c)thread processes

	    sc_thread_handle thread_h = pop_runnable_thread();
	    while( thread_h != 0 ) {
		if ( thread_h->ready_to_run() ) break;
		thread_h = pop_runnable_thread();
	    }
	    if( thread_h != 0 ) {
		m_cor_pkg->yield( thread_h->m_cor_p );
	    }
	    if( m_error ) {
		return;
	    }

	    // check for call(s) to sc_stop
	    if( m_forced_stop ) {
		if ( stop_mode == SC_STOP_IMMEDIATE ) return;
	    }

	    if( m_runnable->is_empty() ) {
		// no more runnable processes
		break;
	    }
	    m_runnable->toggle();
	}

	// UPDATE PHASE
	//
	// The delta count must be updated first so that event_occurred()
	// will work.

	m_execution_phase = phase_update;
	m_delta_count ++;
	m_prim_channel_registry->perform_update();
	m_execution_phase = phase_notify;
	
	if( m_something_to_trace ) {
	    trace_cycle( /* delta cycle? */ true );
	}

	// m_delta_count ++;

        // check for call(s) to sc_stop
        if( m_forced_stop ) {
            break;
        }

	// PROCESS DELTA NOTIFICATIONS:

        int size = m_delta_events.size();
	if ( size != 0 )
	{
	    sc_event** l_events = &m_delta_events[0];
	    int i = size - 1;
	    do {
		l_events[i]->trigger();
	    } while( -- i >= 0 );
	    m_delta_events.resize(0);
	}
	
	if( m_runnable->is_empty() ) {
	    // no more runnable processes
	    break;
	}

	// IF ONLY DOING ONE CYCLE, WE ARE DONE. OTHERWISE GET NEW CALLBACKS

	if ( once ) break;

	m_runnable->toggle();
    } 
}
