
#ifndef SCANRESULTS_H
#define SCANRESULTS_H

#include <QObject>
#include "ui_scanresults.h"

class WpaGui;

class ScanResults : public QDialog, public Ui::ScanResults
{
	Q_OBJECT

public:
	ScanResults(QWidget *parent = 0, const char *name = 0,
		    bool modal = false, Qt::WFlags fl = 0);
	~ScanResults();

public slots:
	virtual void setWpaGui(WpaGui *_wpagui);
	virtual void updateResults();
	virtual void scanRequest();
	virtual void getResults();
	virtual void bssSelected(Q3ListViewItem *sel);

protected slots:
	virtual void languageChange();

private:
	WpaGui *wpagui;
	QTimer *timer;
};

#endif /* SCANRESULTS_H */
