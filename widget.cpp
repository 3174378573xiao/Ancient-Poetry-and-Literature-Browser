#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->setLayout(ui->verticalLayout);
//    ui->scrollAreaWidgetContents->setLayout(ui->verticalLayout_scroll);

    manager = new QNetworkAccessManager(this);

    ui->pushButton_FontUp->resize(64,64);
    ui->pushButton_FontUp->move(200,300);

    ui->listView->setWrapping(true);

    ui->radioButton_shiwen->setChecked(true);
    searchTypeButtonGroup->addButton(ui->radioButton_shiwen,0);
    searchTypeButtonGroup->addButton(ui->radioButton_mingju,1);
    searchTypeButtonGroup->addButton(ui->radioButton_auother,2);
    searchTypeButtonGroup->addButton(ui->radioButton_book,3);
    connect(searchTypeButtonGroup, QOverload<int>::of(&QButtonGroup::buttonClicked),[=](int id){
        this->searchType = (ST)id;
    });
}
Widget::~Widget()
{
    delete ui;
}

QStringList Widget::recFind(QString rec, QString string)
{
    /* 正则表达式 */
    QRegExp re(rec);
    re.setMinimal(true);
    re.setCaseSensitivity(Qt::CaseSensitive);

    /* 输出结果，包含组捕获 */
    QStringList result;
    /* 匹配的偏移位置 */
    int pos = 0;
    while ((pos = re.indexIn(string, pos)) != -1) {
        /* 追加输出结果 */
        result.append(re.capturedTexts()[0]);
        /* 偏移匹配位置 */
        pos += re.matchedLength();
    }
    return result;
}

QStringList Widget::recReplace(QString rec, QStringList string,QString reString)
{
    for (int i=0;i<string.length();i++) {
        auto temp = recFind(rec,string[i]);
        for(auto str:temp){
            string[i].replace(str,reString);
        }
    }
    return string;
}
QStringList Widget::recReplace(QStringList rec, QStringList string, QString reString)
{
    for(QString s:rec){
        for (int i=0;i<string.length();i++) {
            auto temp = recFind(s,string[i]);
            for(auto str:temp){
                string[i].replace(str,reString);
            }
        }
    }
    return string;
}

void Widget::on_pushButton_clicked()
{
    QString input = ui->lineEdit->text();
    if(input.length()!=0){
        QString searchTypeStr;
        switch (searchType) {
        case shiwen:
            searchTypeStr = "shiwen";
            break;
        case mingju:
            searchTypeStr = "mingju";
            break;
        case author:
            searchTypeStr = "author";
            break;
        case book:
            searchTypeStr = "book";
            break;
        }
        QNetworkRequest req_1(QUrl(QString("https://www.gushiwen.cn/search.aspx?type=" + searchTypeStr + "&value="+input+"&valuej="+QString(input[0]))));
        req_1.setSslConfiguration(QSslConfiguration::defaultConfiguration());
        QNetworkRequest req_2(QUrl(QString("https://www.gushiwen.cn/search.aspx?type=" + searchTypeStr + "&page=2&value="+input+"&valuej="+QString(input[0]))));
        req_2.setSslConfiguration(QSslConfiguration::defaultConfiguration());
        QNetworkReply *reply_1 = manager->get(req_1);
        QNetworkReply *reply_2 = manager->get(req_2);
        connect(reply_1,&QNetworkReply::finished,this,&Widget::page_1);
        connect(reply_2,&QNetworkReply::finished,this,&Widget::page_2);
    }
}

void Widget::on_finished()
{
    QString input = (QString)data_1 +(QString)data_2;
    if(data_1.length()==0&&data_2.length()==0){
        QMessageBox msgBox;
        msgBox.setText("搜索失败：网络异常");
        msgBox.exec();
        return;
    }
    input.replace("\n", "");
    input.replace("\r\n", "");
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
//    qDebug().noquote()<<data;

    res_title = out_title(input);
    res_auther = out_auther(input);
    res_text = out_text(input);
    res_dynasty = out_dynasty(input);
    if(res_text.length()!=res_auther.length()||res_text.length()!=res_title.length()||res_text.length()!=res_dynasty.length()){
        QMessageBox msgBox;
        msgBox.setText("搜索失败：无匹配项");
        msgBox.exec();
        return;
    }

    QStringList view;
    int maxL = 0;
    for(int i =0;i<res_title.length();i++){
        if(res_title[i].length()>maxL) maxL = res_title[i].length();
//        qDebug()<<res_title[i].length();
        view.append(res_title[i] + "\n - " + res_auther[i] + res_dynasty[i]);
    }
//    qDebug()<<maxL<<"()";
    ui->scrollAreaWidgetContents->setMinimumSize(maxL*15,res_title.length()*42);

    QStringListModel* model = new QStringListModel(view);
    ui->listView->setModel(model);

}

QStringList Widget::out_title(QString input)
{
    QStringList result = recFind("<b>.*</b>",input);
    result = recReplace({"<.*>","\n","\r\n"},result);
    return result;
}
QStringList Widget::out_auther(QString input)
{
    QStringList result = recFind("<a target=\"_blank\".*>.*</a>",input);
    result = recReplace({"<.*>","\n","\r\n"},result);
    return result;
}
QStringList Widget::out_text(QString input)
{
    QStringList result = recFind("<div class=\"contson\" id=\"contson.*\">.*</div>",input);
    return result;
}
QStringList Widget::out_dynasty(QString input)
{
    QStringList result = recFind("<a href=\"/shiwens/default.aspx\\?cstr=.*>.*</a>",input);
    result = recReplace({"<.*>","\n","\r\n"},result);
    return result;
}

void Widget::showPeom(Widget::peom h)
{
    ui->textBrowser->clear();
    ui->textBrowser->append(h.title);
    ui->textBrowser->append("");
    ui->textBrowser->append(h.auther + h.dynasty);
    ui->textBrowser->append("");
    ui->textBrowser->append(h.text);
}

void Widget::page_1()
{
    data_1.clear();
    QNetworkReply *reply = (QNetworkReply*)(sender());

    data_1 = reply->readAll();
    reply->deleteLater();
    if(data_2.length()!=0){
        on_finished();
    }

}

void Widget::page_2()
{
    data_2.clear();
    QNetworkReply *reply = (QNetworkReply*)(sender());

    data_2 = reply->readAll();
    reply->deleteLater();
    if(data_1.length()!=0){
        on_finished();
    }
}

void Widget::on_listView_clicked(const QModelIndex &index)
{
    peom p;
    p.title = res_title[index.row()];
    p.auther = res_auther[index.row()];
    p.dynasty = res_dynasty[index.row()];
    p.text = res_text[index.row()];

    showPeom(p);
    history.prepend(p);

    QStringList view;
    int maxL = 0;
    for(auto temp:history){
        if(temp.title.length()>maxL) maxL = temp.title.length();
        view.append(temp.title+ "\n - " +temp.auther+temp.dynasty);
    }
    QStringListModel* model = new QStringListModel(view);
    ui->listView_history->setModel(model);
    ui->scrollAreaWidgetContents_history->setMinimumSize(maxL*15,history.length()*42);
}

void Widget::on_pushButton_FontUp_pressed()
{
    ui->textBrowser->zoomIn();
}
void Widget::on_pushButton_FontDown_pressed()
{
    ui->textBrowser->zoomOut();
}
