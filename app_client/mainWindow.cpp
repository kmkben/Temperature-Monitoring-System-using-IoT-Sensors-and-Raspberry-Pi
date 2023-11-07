#include "mainWindow.hpp"

//bool isLedBlinking = false;
//int temperatureThreshold = 30;

Window::Window(int w, int h, QString title) : m_width(w), m_height(h), m_windowTitle(title)
{
	QMenu *menuFile = menuBar()->addMenu("&File");
		
		QAction *actionNew = new QAction("New", this);
			actionNew->setIcon(QIcon("icons/new.png"));

		QAction *actionOpen = new QAction("Open", this);
			actionOpen->setIcon(QIcon("icons/open.png"));

		QAction *actionRecent = new QAction("Recent", this);
		QAction *actionSave = new QAction("Save", this);
		QAction *actionSaveAs = new QAction("Save as", this);
		QAction *actionQuit = new QAction("Quit", this);
			actionQuit->setShortcut(QKeySequence("Ctrl+Q"));
			actionQuit->setIcon(QIcon("icons/quit.png"));
		
		menuFile->addAction(actionNew);
		menuFile->addAction(actionOpen);
		menuFile->addAction(actionRecent);
		menuFile->addAction(actionSave);
		menuFile->addAction(actionSaveAs);
		menuFile->addAction(actionQuit);

	QMenu *menuEdit = menuBar()->addMenu("&Edit");
		
		QAction *actionBold = new QAction("Bold", this);
			actionBold->setShortcut(QKeySequence("Ctrl+B"));
			actionBold->setCheckable(true);

		menuEdit->addAction(actionBold);
		
	/*QMenu *menuInsert = menuBar()->addMenu("&Insert");
		
		QAction *actionImage = new QAction("Image", this);
		QAction *actionForm = new QAction("Forms", this);
		QAction *actionTable = new QAction("Table", this);

		menuInsert->addAction(actionImage);
		menuInsert->addAction(actionForm);
		menuInsert->addAction(actionTable);*/

	QMenu *menuSetting = menuBar()->addMenu("&Setting");

		QAction *actionThemes = new QAction("Themes", this);
		QAction *actionSettings = new QAction("Settings", this);
			actionSettings->setIcon(QIcon("icons/settings.png"));

		menuSetting->addAction(actionThemes);
		menuSetting->addAction(actionSettings);

	QMenu *menuHelp = menuBar()->addMenu("&Help");
		
		QAction *actionHelp = new QAction("FAQ",this);
			actionHelp->setIcon(QIcon("icons/help.png"));

		menuHelp->addAction(actionHelp);

	/*QToolBar *toolBar = addToolBar("File");

		QFontComboBox *choicePolice = new QFontComboBox;

		toolBar->addAction(actionNew);
		toolBar->addAction(actionOpen);
		toolBar->addSeparator();
		toolBar->addWidget(choicePolice);
		toolBar->addSeparator();
		toolBar->addAction(actionSettings);
		toolBar->addAction(actionHelp);
		toolBar->addAction(actionQuit);*/

	QWidget *centralArea = new QWidget;

	setWindowTitle(m_windowTitle);
	setFixedSize(m_width, m_height);

    m_ledBlinking = false;
    m_currentTemperature = 0;
    m_temperatureThreshold = 30;

	setCentralWidget(centralArea);

    QVBoxLayout *mainLayout = new QVBoxLayout;

    m_authGroupBox = new QGroupBox("Authentication"); 
    //m_authGroupBox->setEnabled(false);
    QVBoxLayout *authLayoutv = new QVBoxLayout();
    m_authErrorLabel = new QLabel();
    m_authErrorLabel->setAlignment(Qt::AlignHCenter);
    QHBoxLayout *authLayout = new QHBoxLayout();
        m_serverIPLabel = new QLabel("Server IP:");
        m_serverIPEdit = new QLineEdit();
        m_serverIPEdit->setPlaceholderText("Server IP");
        m_portLabel = new QLabel("Port: ");
        m_portEdit = new QLineEdit();
        m_portEdit->setPlaceholderText("Port");
        m_userEdit = new QLineEdit;
        m_userEdit->setPlaceholderText("Username");

        m_passwordEdit = new QLineEdit;
        m_passwordEdit->setPlaceholderText("Password");
        m_passwordEdit->setEchoMode(QLineEdit::Password);

        m_connectButton = new QPushButton("Connect");
    authLayout->addWidget(m_serverIPLabel);
    authLayout->addWidget(m_serverIPEdit);
    authLayout->addWidget(m_portLabel);
    authLayout->addWidget(m_portEdit);
    authLayout->addWidget(m_connectButton);
    authLayoutv->addLayout(authLayout);
    authLayoutv->addWidget(m_authErrorLabel);
    m_authGroupBox->setLayout(authLayoutv);

    //QGroupBox *led_temperature_groupBox = new QGroupBox();
    QHBoxLayout *led_temperature_layout = new QHBoxLayout;

    m_ledControlGroupBox = new QGroupBox();//("Led blinking control");
    m_ledControlGroupBox->setEnabled(false);
    QHBoxLayout *ledControlLayout = new QHBoxLayout;
        m_ledStartButton = new QPushButton("Start blinking");
        m_ledStopButton = new QPushButton("Stop blinking");
        m_ledStateLabel = new QLabel(m_ledBlinking ? "Led is blinking" : "Led is not blinking");
    ledControlLayout->addWidget(m_ledStartButton);
    ledControlLayout->addWidget(m_ledStopButton);
    ledControlLayout->addWidget(m_ledStateLabel);
    m_ledControlGroupBox->setLayout(ledControlLayout);

    m_currentTemperatureGroupBox = new QGroupBox();
    m_currentTemperatureGroupBox->setEnabled(false);
    QHBoxLayout *currentTemperatureLayout = new QHBoxLayout;
        m_currentTemperatureLabel = new QLabel("Current temperature");
        m_currentTemperatureLCD = new QLCDNumber;
        m_currentTemperatureLCD->setSegmentStyle(QLCDNumber::Flat);
        m_currentTemperatureRefresh = new QPushButton("Refresh");
    currentTemperatureLayout->addWidget(m_currentTemperatureLabel);
    currentTemperatureLayout->addWidget(m_currentTemperatureLCD);
    currentTemperatureLayout->addWidget(m_currentTemperatureRefresh);
    m_currentTemperatureGroupBox->setLayout(currentTemperatureLayout);

    m_temperatureThresholdGroupBox = new QGroupBox();
    m_temperatureThresholdGroupBox->setEnabled(false);
    QHBoxLayout *temperatureThresholdLayout = new QHBoxLayout();
        m_temperatureThresholdLabel = new QLabel("Temperature threshold:");
        m_temperatureThresholdEdit = new QLineEdit();
        m_temperatureThresholdButton = new QPushButton("Change");
    temperatureThresholdLayout->addWidget(m_temperatureThresholdLabel);
    temperatureThresholdLayout->addWidget(m_temperatureThresholdEdit);
    temperatureThresholdLayout->addWidget(m_temperatureThresholdButton);
    m_temperatureThresholdGroupBox->setLayout(temperatureThresholdLayout);

    led_temperature_layout->addWidget(m_currentTemperatureGroupBox);
    //led_temperature_layout->addWidget(m_temperatureThresholdGroupBox);
    led_temperature_layout->addWidget(m_ledControlGroupBox);
    //led_temperature_groupBox->setLayout(led_temperature_layout);

    QHBoxLayout *lcd_temperature_layout = new QHBoxLayout();

    m_lcdControlGroupBox = new QGroupBox();
    m_lcdControlGroupBox->setEnabled(false);
    QHBoxLayout *lcdControlLayout = new QHBoxLayout;
        m_lcdTextLabel = new QLabel("Change LCD Text");
        m_lcdTextEdit = new QLineEdit;
        m_lcdTextEdit->setMaxLength(16);
        m_lcdTextButton = new QPushButton("Change");
    lcdControlLayout->addWidget(m_lcdTextLabel);
    lcdControlLayout->addWidget(m_lcdTextEdit);
    lcdControlLayout->addWidget(m_lcdTextButton);
    m_lcdControlGroupBox->setLayout(lcdControlLayout);

    lcd_temperature_layout->addWidget(m_temperatureThresholdGroupBox);
    lcd_temperature_layout->addWidget(m_lcdControlGroupBox);

    m_historyTemperatureGroupBox = new QGroupBox("Temperature file hidtory");
    m_historyTemperatureGroupBox->setEnabled(false);
    QVBoxLayout *historyTemperatureLayout = new QVBoxLayout();
    m_logGroupBox = new QGroupBox;
    m_logRadioGroupBox = new QGroupBox;
    QHBoxLayout *logLayout = new QHBoxLayout();
    QHBoxLayout *logRadioLayout = new QHBoxLayout();
        m_temperatureLogButton = new QPushButton("Temperature Log");
        m_temperatureLogClearButton = new QPushButton("Clear");
        m_allLogRadioButton = new QRadioButton("All");
        m_allLogRadioButton->setChecked(true);
        m_todayLogRadioButton = new QRadioButton("Only today");
    logRadioLayout->addWidget(m_allLogRadioButton);
    logRadioLayout->addWidget(m_todayLogRadioButton);
    m_logRadioGroupBox->setLayout(logRadioLayout);
    logLayout->addWidget(m_temperatureLogButton);
    logLayout->addWidget(m_temperatureLogClearButton);
    logLayout->addWidget(m_logRadioGroupBox);
    m_logGroupBox->setLayout(logLayout);
        m_historyTemperatureText = new QTextEdit();
        m_historyTemperatureText->setReadOnly(true);
        m_historyTemperatureText->setLineWrapMode(QTextEdit::NoWrap);
    
    historyTemperatureLayout->addWidget(m_logGroupBox);
    historyTemperatureLayout->addWidget(m_historyTemperatureText);
    m_historyTemperatureGroupBox->setLayout(historyTemperatureLayout);

    //mainLayout->addLayout(authLayout);
    mainLayout->addWidget(m_authGroupBox);
    //mainLayout->addLayout(ledControlLayout);
    mainLayout->addLayout(led_temperature_layout);
    //mainLayout->addLayout(lcdControlLayout);
    //mainLayout->addWidget(m_lcdControlGroupBox);
    mainLayout->addLayout(lcd_temperature_layout);
    //mainLayout->addWidget(currentTemperatureGroupBox);
    mainLayout->addWidget(m_historyTemperatureGroupBox);

    centralArea->setLayout(mainLayout);

	connect(actionQuit, SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(m_ledStartButton, SIGNAL(clicked()), this, SLOT(startBlinking()));
    connect(m_ledStopButton, SIGNAL(clicked()), this, SLOT(stopBlinking()));
    connect(m_currentTemperatureRefresh, SIGNAL(clicked()), this, SLOT(refreshCurrentTemperature()));
    connect(m_connectButton, SIGNAL(clicked()), this, SLOT(connectToServer()));
    connect(m_temperatureLogButton, SIGNAL(clicked()), this, SLOT(getTemperatureLog()));
    connect(m_temperatureLogClearButton, SIGNAL(clicked()), this, SLOT(clearTemperatureLog()));
    connect(m_temperatureThresholdButton, SIGNAL(clicked()), this, SLOT(changeTemperatureThresholdValue()));
    connect(m_lcdTextButton, SIGNAL(clicked()), this, SLOT(changeLCDText()));
}



bool Window::isLedBlinking() const
{
    return m_ledBlinking;
}

void Window::setLedBlinking(bool ledBlinking)
{
    m_ledBlinking = ledBlinking;
}

int Window::getTemperatureThreshold() const
{
    return m_temperatureThreshold;
}

void Window::setTemperatureThreshold(int temperature)
{
    m_temperatureThreshold = temperature;
}

int Window::getCurrentTemperature() const
{
    return m_currentTemperature;
}

void Window::setCurrentTemperature(int temperature)
{
    m_currentTemperature = temperature;
}


/*
 *
 * OTHERS FUNCTIONS
 *
 * connection() connects the client to server using the the ip and the port
 *
 */

int Window::connection(QString serverIPStr, int port)
{
    struct sockaddr_in addr;

    //QString serverIPStr = m_serverIPEdit->text();
    const char* serverIP = serverIPStr.toUtf8().constData();

    // Create client socket
    m_client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_client_socket == -1) {
        perror("socket");
        return -1;
    }

    // Configure server address
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(serverIP);

    // Connect to the server
    int res = ::connect(m_client_socket, (struct sockaddr*)&addr, sizeof(addr));
    if (res == -1) {
        perror("connect");
        return -1;
    }

    char temperature[256];
    char threshold[256];

    // Receive temperature
    ssize_t temp_bytes_received = recv(m_client_socket, temperature, sizeof(temperature) - 1, 0);
    if (temp_bytes_received == -1) {
        perror("recv temperature");
        return -1;
    }
    temperature[temp_bytes_received] = '\0';

    std::cout << "temperature received: " << temperature << std::endl;

    // Receive threshold
    ssize_t threshold_bytes_received = recv(m_client_socket, threshold, sizeof(threshold) - 1, 0);
    if (threshold_bytes_received == -1) {
        perror("recv threshold");
        return -1;
    }
    threshold[threshold_bytes_received] = '\0';

    std::cout << "threshold received: " << threshold << std::endl;

    // Convert received strings to integers (if needed)
   QString temperatureStr = temperature;
   QString thresholdStr = threshold;

   bool tempOk, thresholdOk;

   int t = temperatureStr.toInt(&tempOk);
   if (tempOk)
       setCurrentTemperature(t);

   int temperatureThreshold = thresholdStr.toInt(&thresholdOk);
   if (thresholdOk)
       setTemperatureThreshold(temperatureThreshold);

    
    return 0;
}

int Window::sendRequest(const char* command)
{
    int bytes_written = write(m_client_socket, command, strlen(command));
    if (bytes_written == -1) {
        perror("write");
        return -1;
    }
    return 0;
}

/*
 *
 * CUSTOM SLOTS 
 *
 */

void Window::connectToServer()
{
    if(!m_serverIPEdit->text().isEmpty() && !m_portEdit->text().isEmpty())
    {
        QString serverIP = m_serverIPEdit->text();
        //const char* serverIP = serverIPStr.toUtf8().constData();
        QString portStr = m_portEdit->text();
        bool ok;
        int port = portStr.toInt(&ok);

        if(ok)
        {
            if(connection(serverIP, port) == -1)
            {
                m_authErrorLabel->setStyleSheet("color: red;");
                m_authErrorLabel->setText("Error: Connection refused");

            }
            else 
            {
                m_authErrorLabel->setStyleSheet("color: green;");
                m_authErrorLabel->setText("Connected");

                m_currentTemperatureGroupBox->setEnabled(true);
                m_ledControlGroupBox->setEnabled(true);
                m_temperatureThresholdGroupBox->setEnabled(true);
                m_lcdControlGroupBox->setEnabled(true);
                m_historyTemperatureGroupBox->setEnabled(true);

                m_currentTemperatureLCD->display(m_currentTemperature);
                m_lcdTextEdit->setText("Welcome !");
                QString th = QString::number(m_temperatureThreshold);
                //th += m_temperatureThreshold;
                m_temperatureThresholdEdit->setText(th);

            }
        }
        else
        {
            m_authErrorLabel->setStyleSheet("color: red;");
            m_authErrorLabel->setText("Error: Invalid port number");
        }
    }
    else
    {
        m_authErrorLabel->setStyleSheet("color: red;");
        m_authErrorLabel->setText("Error: You should fill the IP and the port");
    }
}


void Window::refreshCurrentTemperature()
{
    sendRequest("temperature");

    char temperature[256];

    ssize_t bytes_received = recv(m_client_socket, temperature, sizeof(temperature) - 1, 0);
    if (bytes_received == -1) {
        perror("recv");
        return;
    }
    temperature[bytes_received] = '\0';

    QString temperatureStr = temperature;
    bool ok;
    int t = temperatureStr.toInt(&ok);

    if(ok)
    {
        setCurrentTemperature(t);
    }

    m_currentTemperatureLCD->display(m_currentTemperature);

}

void Window::startBlinking()
{
    if (!isLedBlinking())
    {
        sendRequest("start");
        m_ledBlinking = true;
        //m_ledControlButton->setText("Start Blinking LED");
        m_ledStateLabel->setText("LED is blinking.");
    }
    /*else
    {
        m_ledBlinking = true;
        m_ledControlButton->setText("Stop Blinking LED");
        m_ledStateLabel->setText("LED is blinking.");
    }*/
}

void Window::stopBlinking()
{
    if (isLedBlinking())
    {
        sendRequest("stop");
        m_ledBlinking = false;
        //m_ledControlButton->setText("Start Blinking LED");
        m_ledStateLabel->setText("LED is not blinking.");
    }
    /*else
    {
        m_ledBlinking = true;
        m_ledControlButton->setText("Stop Blinking LED");
        m_ledStateLabel->setText("LED is blinking.");
    }*/

}

void Window::changeLCDText()
{
    QString str = m_lcdTextEdit->text();
    str += " ";
    QByteArray byteArray = str.toUtf8();
    const char* request = byteArray.constData();
    sendRequest(request);
}

void Window::changeTemperatureThresholdValue()
{
    QString str = m_temperatureThresholdEdit->text();
    //str += " ";
    QByteArray byteArray = str.toUtf8();
    const char* request = byteArray.constData();
    sendRequest(request);
}

void Window::getTemperatureLog()
{
    sendRequest("log");
    QString temperature = ""; 

    char buff[2650000];

    ssize_t bytes_received = recv(m_client_socket, buff, sizeof(buff) - 1, 0);
    if (bytes_received == -1) {
        perror("recv");
        return;
    }
    buff[bytes_received] = '\0';

    temperature = buff;
    

    m_historyTemperatureText->setPlainText(temperature);
}

void Window::clearTemperatureLog()
{
    m_historyTemperatureText->setPlainText("");
}
