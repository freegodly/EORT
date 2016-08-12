#ifndef DIALOG_SETTING_H
#define DIALOG_SETTING_H

#include <QDialog>
#include <QDomElement>
#include <QCloseEvent>

namespace Ui {
class Dialog_Setting;
}

class Dialog_Setting : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_Setting(QWidget *parent = 0);
    ~Dialog_Setting();
       //************************************
        // Method:    	SaveConfig
        // FullName:  	ConfigWindow::SaveConfig
        // Access:    	private
        // Returns:   	void
        // Description:
        // Qualifier:
        // Date:		2014-4-11 created by NingJian
        //************************************
        void SaveConfig();

       //************************************
       // Method:    	LoadConfig
       // FullName:  	ConfigWindow::LoadConfig
       // Access:    	private
       // Returns:   	void
       // Description:
       // Qualifier:
       // Parameter: 	const QObjectList & q
       // Date:		2014-4-11 created by NingJian
       //************************************
       void LoadConfig(const QObjectList& q);
       //************************************
       // Method:    	traversalControl
       // FullName:  	ConfigWindow::traversalControl
       // Access:    	private
       // Returns:   	void
       // Description:
       // Qualifier:
       // Parameter: 	const QObjectList & q
       // Parameter: 	QDomElement * root
       // Date:		2014-4-11 created by NingJian
       //************************************
       void traversalControl(const QObjectList& q,QDomElement* root) ;


       void Load_Profile();

protected:
       void closeEvent(QCloseEvent *event);

private slots:
       void on_tabWidget_destroyed();

       void on_pushButton_SelectFile_clicked();

       void on_pushButton_SelectFile_File_clicked();

        void contextMenuRequest(QPoint pos);

private:
    Ui::Dialog_Setting *ui;
};

#endif // DIALOG_SETTING_H
