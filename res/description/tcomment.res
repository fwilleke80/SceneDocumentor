/////////////////////////////////////////////////////////////
//
// SceneDocumentor :: Comment Tag Resource
//
/////////////////////////////////////////////////////////////
//
// Comment Tag :: Main Tag Description Resource
//
/////////////////////////////////////////////////////////////
// 2009-2010 by www.c4d-jack.de
// OpenSource. No warranty, no support.
/////////////////////////////////////////////////////////////

CONTAINER tComment
{
	NAME tComment;

	GROUP COMMENT_PROPERTIES
	{
		DEFAULT 1;
		
		LONG		COMMENT_ICONMODE	{	CYCLE {
																				COMMENT_ICONMODE_0;
																				COMMENT_ICONMODE_1;
																				COMMENT_ICONMODE_2;
																				COMMENT_ICONMODE_3;
																				COMMENT_ICONMODE_4;
																				COMMENT_ICONMODE_5;
																			}
															}
		SEPARATOR									{  }
		STRING	COMMENT_TITLE			{  }
		STRING	COMMENT_TEXT			{ CUSTOMGUI MULTISTRING; }
		STATICTEXT	COMMENT_HELPTEXT	{ ANIM OFF; }
		
		SEPARATOR									{  }
		
		BUTTON	COMMENT_CREATEOBJ	{  }
		
		SEPARATOR									{  }
		
		BUTTON	COMMENT_OPENHELP	{  }
	}
}
