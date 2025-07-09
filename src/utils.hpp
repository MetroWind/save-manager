#include <QMessageBox>

inline void msgBox(const char* msg)
{
    QMessageBox msgBox;
    msgBox.setText(msg);
    msgBox.exec();
}
