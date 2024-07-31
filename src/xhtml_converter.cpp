#include "qdebug.h"
#include <tidy.h>
#include <tidybuffio.h>
#include <xhtml_converter.hpp>

template<typename T>
struct scope_guard {
	T callback;
	scope_guard(T func)
		: callback {func} {}
	~scope_guard() {
		callback();
	}
};

QString html_to_xhtml(const QString& html) {
	TidyBuffer output;
	tidyBufInit(&output);
	TidyBuffer errbuf = {0};
	tidyBufInit(&errbuf);
	TidyDoc tdoc = tidyCreate();
	auto guard = scope_guard([&output, &errbuf, &tdoc](){
		tidyBufFree(&output);
		tidyBufFree(&errbuf);
		tidyRelease(tdoc);
	});
	int rc = -1;
	bool ok = true;
	Bool test;

	ok = ok && tidyOptParseValue( tdoc, "new-empty-tags", "amp-state, amp-img, amp-accordion, amp-list, amp-user-notification, amp-sidebar" );

	ok = ok && tidyOptSetBool ( tdoc, TidyXhtmlOut        , yes   ); // Boolean
	ok = ok && tidyOptSetBool ( tdoc, TidyShowWarnings    , no    ); // Boolean
	ok = ok && tidyOptSetBool ( tdoc, TidyIndentAttributes, no    ); // Boolean
	ok = ok && tidyOptSetBool ( tdoc, TidyIndentCdata     , no    ); // Boolean
	ok = ok && tidyOptSetBool ( tdoc, TidyQuiet           , yes   ); // Boolean
	ok = ok && tidyOptSetBool ( tdoc, TidyOmitOptionalTags, yes   ); // Boolean
	ok = ok && tidyOptSetBool ( tdoc, TidyDropEmptyElems  , yes   ); // Boolean
	ok = ok && tidyOptSetBool ( tdoc, TidyDropEmptyParas  , yes   ); // Boolean
	ok = ok && tidyOptSetBool ( tdoc, TidyHideComments    , yes   ); // Boolean
	ok = ok && tidyOptSetValue( tdoc, TidyMergeDivs       , "yes" ); // Enum
	ok = ok && tidyOptSetBool ( tdoc, TidyMergeEmphasis   , yes   ); // Boolean
	ok = ok && tidyOptSetValue( tdoc, TidyMergeSpans      , "yes" ); // Enum
	ok = ok && tidyOptSetBool ( tdoc, TidyQuoteNbsp       , no    ); // Boolean
	ok = ok && tidyOptSetInt  ( tdoc, TidyWrapLen         , 0     ); // Integer

	if ( !ok )
		throw std::runtime_error("Failed to set tidy-html parameters");
	rc = tidySetErrorBuffer( tdoc, &errbuf );                     // Capture diagnostics
	if ( rc >= 0 )
		rc = tidyParseString( tdoc, html.toUtf8().constData() );  // Prse the input
	if ( rc >= 0 )
		rc = tidyCleanAndRepair( tdoc );                          // Tdy it up!
	if ( rc >= 0 )
		rc = tidyRunDiagnostics( tdoc );                          // Kvetch
	if ( rc > 1 )                                                 // If error, force output.
		rc = ( tidyOptSetBool(tdoc, TidyForceOutput, yes) ? rc : -1 );
	if ( rc >= 0 )
		rc = tidySaveBuffer( tdoc, &output );                     // Pretty Print
	if ( rc >= 0 ) {
		if ( rc > 0 )
			qDebug() << QString::fromUtf8(errbuf.bp);
		return QString::fromUtf8(output.bp);
	}
	throw std::runtime_error(QString("A severe error occurred while converting HTML to XHTML. Return code: %1").arg(rc).toUtf8().constData());
}
