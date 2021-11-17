#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql>
#include <QSqlDatabase>
#include <QMessageBox>
#include <wiringPiI2C.h>
#include <QDebug>
#include "pi2c.h"
#include <QThread>
#include <QTimer>

#define DEVICE_ID 0x18

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_toolButton_9_triggered(QAction *arg1);

    void on_toolButton_9_clicked();

    void on_toolButton_3_clicked();

    void on_pushButton_110_clicked();

    void on_toolButton_25_clicked();

    void on_toolButton_16_clicked();

    void on_toolButton_15_clicked();

    void on_toolButton_6_clicked();

    void on_toolButton_clicked();

    void on_toolButton_2_clicked();

    void on_toolButton_26_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_32_clicked();

    void on_pushButton_66_clicked();

    void on_pushButton_26_clicked();

    void on_pushButton_95_clicked();

    void on_pushButton_99_clicked();

    void on_pushButton_104_clicked();

    void on_pushButton_88_clicked();

    void on_pushButton_136_clicked();

    void on_pushButton_139_clicked();

    void on_pushButton_111_clicked();

    void on_pushButton_112_clicked();

    void on_pushButton_113_clicked();

    void on_pushButton_117_clicked();

    void on_pushButton_118_clicked();

    void on_pushButton_119_clicked();

    void on_pushButton_120_clicked();

    void on_pushButton_122_clicked();

    void on_pushButton_129_clicked();

    void on_pushButton_124_clicked();

    void on_pushButton_127_clicked();

    void on_pushButton_126_clicked();

    void on_pushButton_130_clicked();

    void on_pushButton_121_clicked();

    void on_pushButton_131_clicked();

    void on_pushButton_132_clicked();

    void on_pushButton_142_clicked();

    void on_pushButton_135_clicked();

    void on_pushButton_133_clicked();

    void on_pushButton_144_clicked();

    void on_pushButton_143_clicked();

    void on_pushButton_146_clicked();

    void on_pushButton_145_clicked();

    void on_pushButton_149_clicked();

    void on_pushButton_148_clicked();

    void on_pushButton_147_clicked();

    void on_pushButton_151_clicked();

    void on_pushButton_150_clicked();

    void on_pushButton_153_clicked();

    void on_pushButton_152_clicked();

    void on_pushButton_156_clicked();

    void on_pushButton_155_clicked();

    void on_pushButton_154_clicked();

    void on_pushButton_158_clicked();

    void on_pushButton_157_clicked();

    void on_pushButton_164_clicked();

    void on_pushButton_162_clicked();

    void on_pushButton_161_clicked();

    void on_pushButton_159_clicked();

    void on_pushButton_160_clicked();

    void on_pushButton_163_clicked();

    void on_pushButton_165_clicked();

    void on_pushButton_166_clicked();

    void on_pushButton_clicked();

    void on_pushButton_15_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_14_clicked();

    void on_pushButton_12_clicked();

    void on_pushButton_13_clicked();

    void on_pushButton_11_clicked();

    void on_pushButton_18_clicked();

    void on_pushButton_16_clicked();

    void on_pushButton_27_clicked();

    void on_pushButton_20_clicked();

    void on_pushButton_21_clicked();

    void on_pushButton_19_clicked();

    void on_pushButton_28_clicked();

    void on_pushButton_25_clicked();

    void on_pushButton_22_clicked();

    void on_pushButton_23_clicked();

    void on_pushButton_29_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_8_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_33_clicked();

    void on_pushButton_34_clicked();

    void on_pushButton_30_clicked();

    void on_pushButton_31_clicked();

    void on_pushButton_24_clicked();

    void on_pushButton_17_clicked();

    void on_pushButton_68_clicked();

    void on_pushButton_69_clicked();

    void on_pushButton_67_clicked();

    void on_pushButton_65_clicked();

    void on_pushButton_70_clicked();

    void on_pushButton_79_clicked();

    void on_pushButton_109_clicked();

    void on_pushButton_71_clicked();

    void on_pushButton_72_clicked();

    void on_pushButton_74_clicked();

    void on_pushButton_73_clicked();

    void on_pushButton_78_clicked();

    void on_pushButton_76_clicked();

    void on_pushButton_77_clicked();

    void on_pushButton_75_clicked();

    void on_pushButton_81_clicked();

    void on_pushButton_80_clicked();

    void on_pushButton_89_clicked();

    void on_pushButton_83_clicked();

    void on_pushButton_84_clicked();

    void on_pushButton_82_clicked();

    void on_pushButton_90_clicked();

    void on_pushButton_87_clicked();

    void on_pushButton_85_clicked();

    void on_pushButton_86_clicked();

    void on_pushButton_91_clicked();

    void on_pushButton_101_clicked();

    void on_pushButton_98_clicked();

    void on_pushButton_93_clicked();

    void on_pushButton_100_clicked();

    void on_pushButton_102_clicked();

    void on_pushButton_96_clicked();

    void on_pushButton_97_clicked();

    void on_pushButton_92_clicked();

    void on_pushButton_94_clicked();

    void on_pushButton_106_clicked();

    void on_pushButton_107_clicked();

    void on_pushButton_105_clicked();

    void on_pushButton_103_clicked();

    void on_pushButton_108_clicked();

    void on_pushButton_116_clicked();

    void on_pushButton_114_clicked();

    void on_pushButton_115_clicked();

    void on_pushButton_128_clicked();

    void on_pushButton_125_clicked();

    void on_pushButton_123_clicked();

    void on_pushButton_138_clicked();

    void on_pushButton_140_clicked();

    void on_pushButton_134_clicked();

    void on_pushButton_141_clicked();

    void on_pushButton_137_clicked();

    void on_toolButton_5_clicked();

    void on_toolButton_4_clicked();

    void realtime_temperature();
    void testing();
    void processing();
    void init_motor();
    void init();
    void uv_timer();
    void proc_timer();

    void on_toolButton_45_clicked();

    void on_toolButton_46_clicked();

    void on_toolButton_18_clicked();

    void on_toolButton_43_clicked();

    void on_toolButton_7_clicked();

private:
    Ui::MainWindow *ui;
    QTimer *timer, *timer1, *timer2, *timer3, *uvtimer, *proctimer;
};

#endif // MAINWINDOW_H
