#ifndef WINDOW_HPP
#define WINDOW_HPP


#include <QtWidgets>
#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>

//extern bool isLedBlinking;
//extern int temperatureThreshold;

class Window : public QMainWindow
{
    Q_OBJECT
 
	public:
		Window(int, int, QString);

        bool isLedBlinking() const;
        void setLedBlinking(bool);
        int getTemperatureThreshold() const;
        void setTemperatureThreshold(int);
        int getCurrentTemperature() const;
        void setCurrentTemperature(int);

        int connection(QString, int);
        int sendRequest(const char*);

    private slots:
        void connectToServer();
        void refreshCurrentTemperature();
        void startBlinking();
        void stopBlinking();
        void getTemperatureLog();
        void clearTemperatureLog();
        void changeLCDText();
        void changeTemperatureThresholdValue();

	private:

        bool m_ledBlinking;
        int m_temperatureThreshold;
        int m_currentTemperature;

		int m_width;
		int m_height;
		QString m_windowTitle;

        QLineEdit *m_serverIPEdit;
        QLineEdit *m_portEdit;
        QLineEdit *m_userEdit;
        QLineEdit *m_passwordEdit;
        QLineEdit *m_lcdTextEdit;
        QLineEdit *m_temperatureThresholdEdit;

        QTextEdit *m_historyTemperatureText;

        QPushButton *m_connectButton;
        QPushButton *m_ledStartButton;
        QPushButton *m_ledStopButton;
        QPushButton *m_lcdTextButton;
        QPushButton *m_currentTemperatureRefresh;
        QPushButton *m_temperatureThresholdButton;
        QPushButton *m_temperatureLogButton;
        QPushButton *m_temperatureLogClearButton;

        QRadioButton *m_allLogRadioButton;
        QRadioButton *m_todayLogRadioButton;

        QLabel *m_authErrorLabel;
        QLabel *m_serverIPLabel;
        QLabel *m_portLabel;
        QLabel *m_ledStateLabel;
        QLabel *m_lcdTextLabel;
        QLabel *m_lcdCurrentText;
        QLabel *m_currentTemperatureLabel;
        QLabel *m_temperatureThresholdLabel;

        QLCDNumber *m_currentTemperatureLCD;

        QGroupBox *m_authGroupBox;
        QGroupBox *m_currentTemperatureGroupBox;
        QGroupBox *m_ledControlGroupBox;
        QGroupBox *m_temperatureThresholdGroupBox;
        QGroupBox *m_historyTemperatureGroupBox;
        QGroupBox *m_lcdControlGroupBox;
        QGroupBox *m_logGroupBox;
        QGroupBox *m_logRadioGroupBox;
        

        int m_client_socket;

};


#endif //WINDOW_HPP
