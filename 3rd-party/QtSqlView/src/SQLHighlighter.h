// $Id: SQLHighlighter.h 20 2006-10-10 08:24:36Z tb $
// Copyright 2006 Timo Bingmann
//
// SQL syntax highlight plugin for QTextEdit
//

#ifndef _SQLHighlighter_H_
#define _SQLHighlighter_H_

#include <QtCore/QVector>
#include <QtCore/QRegExp>

#include <QtGui/QSyntaxHighlighter>
#include <QtGui/QTextCharFormat>

class SQLHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    SQLHighlighter(class QTextDocument *parent = NULL);

protected:
    virtual void highlightBlock(const QString &text);

private:
    struct Rule
    {
	QRegExp 	pattern;
	QTextCharFormat format;

	inline Rule()
	{ }

	inline Rule(QString p, QTextCharFormat f)
	    : pattern(p, Qt::CaseInsensitive), format(f)
	{ }
    };

    QVector<Rule> 	rules;

    QRegExp 		commentStartExpression;
    QRegExp 		commentEndExpression;

    QTextCharFormat 	commentFormat;
};

#endif // _SQLHighlighter_H_
