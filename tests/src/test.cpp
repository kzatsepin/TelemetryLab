#include <QtTest>

class TelemetrySmokeTest : public QObject {
    Q_OBJECT
private slots:
    void initTestCase() {
        QVERIFY(true);
    }
};

QTEST_MAIN(TelemetrySmokeTest)
#include "test.moc"
