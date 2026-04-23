#include "EnglishLearner.h"

#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    EnglishLearner window;
    window.show();

    return app.exec();
}
