public enum class SC_DIR {
	SCD_FORWARD,				// move forward in moviw
	SCD_BACK,					// move back in movie
	SCD_NONE					// no change
};

public enum class SC_STATE {
	SCS_PLAY_REC,				// playing & recording
	SCS_PLAY_NOREC,				// playing & no recording
	SCS_NOPLAY_REC,				// no playing & recording
	SCS_NOPLAY_NOREC,			// no playing & no recording
	SCS_NONE,					// no playing & no recording
};

public enum class SC_SEQ {
	SCQ_ADDED,					// added an event
	SCQ_REMOVED,				// removed an event
	SCQ_CHANGED,				// changed an event
	SCQ_LOADED,				    // loaded event sequence
	SCQ_NEW,				    // created new sequence
	SCQ_NONE,					// no event change
};

public enum class SC_EVENT {
	SCE_MOVED_AFTER,			// just moved past an event (should be given)
	SCE_MOVED_BEFORE,			// just moved before an event (should be given)
	SCE_PAST_LAST,				// moved past last existing event
	SCE_BEFORE_FIRST,			// moved before first event
	SCE_NONE,					// no event move change
};

public enum class SC_REASON {
	SCR_PLAYING,				// changed because of playing movie
	SCR_SCROLLING,				// changed because of scrolling movie
	SCR_STEP,					// changed because of stepping (button)
	SCR_GRID,					// changed because of selecting on grid
	SCR_NUMPAD,					// change because of numberpad
	SCR_LOADSEQ,				// changed because of loading sequence
	SCR_NEWSEQ,				// changed because of loading sequence
	SCR_NONE,					// none (should not happen??)
};

//public enum class SC_INDEX {
//	SCI_ORIGIN = 0,				// indexes origin
//	SCI_DEST   = 1,				// indexes destination
//	SCI_NEXT   = 2,				// indexes next
//};
