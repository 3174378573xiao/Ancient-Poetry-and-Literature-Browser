#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QDomDocument>
#include <QHostInfo>
#include <QMessageBox>
#include <QNetworkConfigurationManager>
#include <QStringListModel>
#include <QTextCodec>
#include <iostream>
#include <string>
#include <QButtonGroup>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    QNetworkAccessManager *manager;
    QStringList recFind(QString rec,QString string);
    QStringList recReplace(QString rec, QStringList string,QString reString="");
    QStringList recReplace(QStringList rec, QStringList string,QString reString="");

    QStringList res_title;
    QStringList out_title(QString input);


    QStringList res_auther;
    QStringList out_auther(QString input);

    QStringList res_text;
    QStringList out_text(QString input);

    QStringList res_dynasty;
    QStringList out_dynasty(QString input);

    struct peom{
        QString title,auther,text,dynasty;
    };
    QList<peom> history;
    void showPeom(peom h);

    enum ST{
        shiwen=0,mingju=1,author=2,book=3
    }searchType;

    QButtonGroup* searchTypeButtonGroup = new QButtonGroup(this);

    QByteArray data_1,data_2;

private slots:

    void page_1();
    void page_2();

    void on_pushButton_clicked();
    void on_finished();

    void on_listView_clicked(const QModelIndex &index);

    void on_pushButton_FontUp_pressed();

    void on_pushButton_FontDown_pressed();

private:
    Ui::Widget *ui;
};
#endif // WIDGET_H
