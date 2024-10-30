/*Author: CHOUBRI Douae*/

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    Object_GL = new ObjectOpenGL();
    Object_GL->setObjectName(QString::fromUtf8("ObjectOpenGL"));
    Object_GL->setGeometry(QRect(0, 0, this->width(), this->width()));

    ui->gridLayout->addWidget(Object_GL, 2, 0, 1, 2);

    // Create the menubar
    QMenu *FileMenu = menuBar()->addMenu("&File");
    FileMenu->addSeparator();
    FileMenu->addAction("Quit", qApp, SLOT (quit()), QKeySequence(tr("Ctrl+Q")));

    openCANPort();
    setupTimers();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_OnOffButton_clicked()
{
    scpp::CanFrame frame_to_write;
    frame_to_write.id = 0x03;
    frame_to_write.len = 1;
    if(ui->OnOffButton->text() == "ON")
    {
        ui->OnOffButton->setText("OFF");
        ui->OnOffButton->setStyleSheet("background-color: red");
    }
    else
    {
        ui->OnOffButton->setText("ON");
        ui->OnOffButton->setStyleSheet("background-color: green");
    }
    frame_to_write.data[0] = 0x30; // Data for OFF/ON state
    socket_can.write(frame_to_write);
}


void MainWindow::on_DistanceRadio_clicked()
{
    scpp::CanFrame frame_to_write;
    if(ui->DistanceRadio->isChecked())
    {
        ui->uniteLabel->setText("mm");
        ui->DistanceBrightnessValue->setText("0");
        frame_to_write.id = 0x01;
        frame_to_write.len = 1;
        frame_to_write.data[0] = 0x20; // Code pour la distance
    }
    else
    {
        ui->uniteLabel->setText("Lux");
        ui->DistanceBrightnessValue->setText("0");
        frame_to_write.id = 0x01;
        frame_to_write.len = 1;
        frame_to_write.data[0] = 0x10; // Code pour la luminosité
    }
    socket_can.write(frame_to_write);
}

void MainWindow::on_BrightnessRadio_clicked()
{
    scpp::CanFrame frame_to_write;
    if(ui->BrightnessRadio->isChecked())
    {
        ui->uniteLabel->setText("Lux");
        ui->DistanceBrightnessValue->setText("0");
        frame_to_write.id = 0x01;
        frame_to_write.len = 1;
        frame_to_write.data[0] = 0x10; // Code pour la luminosité
    }
    else
    {
        ui->uniteLabel->setText("mm");
        ui->DistanceBrightnessValue->setText("0");
        frame_to_write.id = 0x01;
        frame_to_write.len = 1;
        frame_to_write.data[0] = 0x20; // Code pour la distance
    }
    socket_can.write(frame_to_write);
}

//OpenCAN port

void MainWindow::openCANPort()
{

   if (socket_can.open("can0") == scpp::STATUS_OK)
   {
        printf("can socket opened");
   }
   else
   {
       printf("Cannot open can socket!");
   }

}

void MainWindow::on_setSpeed_clicked()
{
    scpp::CanFrame frame_to_write;

    frame_to_write.id = 0x3; 
    frame_to_write.len = 2;
    int motorSpeed = ui->MotorSpeed->text().toInt();
    if(motorSpeed >= 0 && motorSpeed <= 200)
    {
        frame_to_write.data[0] = 0x40;  
        frame_to_write.data[1] = static_cast<uint8_t>(motorSpeed); 
    }
    else
    {
        frame_to_write.data[0] = 0;  
        frame_to_write.data[1] = 0;
    }
    // Send CAN frame
    socket_can.write(frame_to_write);
    std::cout << "Speed (hex) = 0x" << std::hex << motorSpeed << std::endl;
}

void MainWindow::on_MotorSpeed_2_clicked()
{
    // Initialiser le générateur de nombres aléatoires une seule fois
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    
    // Générer une vitesse aléatoire entre 30 et 200
    int motorSpeed = 30 + std::rand() % (200 - 30 + 1);
    
    // Afficher la vitesse générée dans le bouton
    ui->MotorSpeed_2->setText(QString::number(motorSpeed));
    
    // Préparer et envoyer la trame CAN
    scpp::CanFrame frame_to_write;
    frame_to_write.id = 0x3;
    frame_to_write.len = 2;
    frame_to_write.data[0] = 0x40;
    frame_to_write.data[1] = static_cast<uint8_t>(motorSpeed);
    
    socket_can.write(frame_to_write);

    // Afficher la vitesse dans la console pour vérification
    std::cout << "Vitesse aléatoire (hex) = 0x" << std::hex << motorSpeed << std::endl;
}


void MainWindow::handleAnemometerData(const scpp::CanFrame& fr)
{
    uint8_t Spw = (fr.data[0]);
    ui->windSpeedLabel->setText(QString::number(Spw));
}

void MainWindow::handleLuxTempHumData(const scpp::CanFrame& fr)
{
    // Récupération de la valeur de luminosité (3 octets : octets [0], [1], [2]) si nécessaire
    uint32_t lux = (fr.data[2] << 16) | (fr.data[1] << 8) | fr.data[0];
    ui->DistanceBrightnessValue->setText(QString::number(lux));

    // Récupération de la pression (2 octets : octets [3], [4])
    uint16_t Pa = (fr.data[4] << 8) | fr.data[3];
    float pressure = Pa / 10.0f;  // Diviser par 10 pour revenir en hPa
    ui->PressureLabel->setText(QString::number(pressure, 'f', 1));

    // Récupération de la température (2 octets : octets [5], [6])
    uint16_t temp = (fr.data[6] << 8) | fr.data[5];
    float temperature = temp/100.0f;  // Diviser par 100 pour avoir les degrés Celsius (°C)
    ui->temperaturelabel->setText(QString::number(temperature, 'f', 2));

    // Récupération de l'humidité (1 octet : octet [7])
    uint8_t hum = fr.data[7];
    float humidity = hum; 
    ui->HumidityLabel->setText(QString::number(humidity, 'f', 0));
}

void MainWindow::handleAccelData(const scpp::CanFrame& fr)
{
    // Extraction des données brutes pour l'accélération (x, y, z)
    int16_t acc_x_raw = (fr.data[1] << 8) | fr.data[0];
    int16_t acc_y_raw = (fr.data[3] << 8) | fr.data[2];
    int16_t acc_z_raw = (fr.data[5] << 8) | fr.data[4];

    // Application du facteur d'échelle (4.0 / 32768.0)
    float acc_x = acc_x_raw * (4.0f / 32768.0f);
    float acc_y = acc_y_raw * (4.0f / 32768.0f);
    float acc_z = acc_z_raw * (4.0f / 32768.0f);

    // Affichage des valeurs dans l'interface utilisateur
    ui->accelerationxLabel->setText(QString::number(acc_x, 'f', 2));
    ui->accelerationyLabel->setText(QString::number(acc_y, 'f', 2));
    ui->accelerationzLabel->setText(QString::number(acc_z, 'f', 2));
}

void MainWindow::handleGyroData(const scpp::CanFrame& fr)
{
    // Extraction des données brutes pour le gyroscope (x, y, z)
    int16_t gyr_x_raw = (fr.data[1] << 8) | fr.data[0];
    int16_t gyr_y_raw = (fr.data[3] << 8) | fr.data[2];
    int16_t gyr_z_raw = (fr.data[5] << 8) | fr.data[4];

    // Application du facteur d'échelle pour obtenir des radians par seconde
    float gyr_x = gyr_x_raw * ((1000.0f / 32768.0f) * (M_PI / 180.0f)); // Conversion en rad/s
    float gyr_y = gyr_y_raw * ((1000.0f / 32768.0f) * (M_PI / 180.0f)); // Conversion en rad/s
    float gyr_z = gyr_z_raw * ((1000.0f / 32768.0f) * (M_PI / 180.0f)); // Conversion en rad/s

    // Affichage des valeurs dans l'interface utilisateur
    ui->gyroscopexLabel->setText(QString::number(gyr_x, 'f', 2));
    ui->gyroscopeyLabel->setText(QString::number(gyr_y, 'f', 2));
    ui->gyroscopezLabel->setText(QString::number(gyr_z, 'f', 2));
}

void MainWindow::handleOtherSensorData(const scpp::CanFrame& fr)
{
    if (fr.id == 0x95)
    {
        int16_t phi =  (fr.data[1] << 8) | fr.data[0];       
        int16_t theta = (fr.data[3] << 8) | fr.data[2];
        int16_t psi = (fr.data[5] << 8) | fr.data[4];

        float phi_deg = static_cast<float> (phi) / 100.0f;
        float theta_deg = static_cast<float>(theta) / 100.0f;
        float psi_deg = static_cast<float>(psi) / 100.0f;

        phi_deg = phi_deg * (180.0f / 3.14f);
        theta_deg = theta_deg * (180.0f/ 3.14f);    
        psi_deg = psi_deg * (180.0f / 3.14f);    


        // Affichage dans les labels UI
        ui->phiLabel->setText(QString::number(phi_deg, 'f', 2));      
        ui->ThetaLabel->setText(QString::number(theta_deg, 'f', 2));
        ui->PsiLabel->setText(QString::number(psi_deg, 'f', 2));

        // Mise à jour des angles pour l'objet OpenGL
        Object_GL->SetXRotation((-phi_deg)*16);
        Object_GL->SetYRotation((theta_deg)*16);
        Object_GL->SetZRotation((-psi_deg)*16);
        //Object_GL->setAngles(-phi_deg,theta_deg,psi_deg);
        Object_GL->updateGL();

        handleAccelData(fr);
        handleGyroData(fr);
    }
}

/////////

void MainWindow::receiveCANMessage()
{
    scpp::CanFrame fr;

    if (socket_can.read(fr) == scpp::STATUS_OK)
    {
        if (fr.id == 0x85)
        {
            handleAnemometerData(fr);
        }
        else if (fr.id == 0x75)
        {
            handleLuxTempHumData(fr);
        }
        else
        {
            handleOtherSensorData(fr);
        }
    }
}


void MainWindow::onTimer_Tick()
{
    receiveCANMessage();
    //Object_GL->updateGL();
}

void MainWindow::onRefresh_Tick()
{
    // Timer dédié à l'envoi périodique de demandes CAN
    switch (sendTurn) {
    case 0:
        sendOrder(0x01);  // Demande pour la carte ID_1
        break;
    case 1:
        sendOrder(0x03);  // Demande pour l'anémomètre (ID 3)
        break;
    default:
        break;
    }
    sendTurn = (sendTurn + 1) % 2;
}

void MainWindow::onAccelTimer_Tick()
{
    // Timer dédié à l'envoi des demandes de l'accéléromètre uniquement
    sendOrder(0x02);  // ID pour l'accéléromètre id 0x02
}

void MainWindow::sendOrder(int id) {
    scpp::CanFrame frame_to_write;
    frame_to_write.id   = id;
    frame_to_write.len  = 1;  
    frame_to_write.data[0] = id;
    socket_can.write(frame_to_write);
}

void MainWindow::setupTimers()
{
    timer_tick = new QTimer(this);  // Timer principal pour la mise à jour générale
    connect(timer_tick, &QTimer::timeout, this, &MainWindow::onTimer_Tick);
    timer_tick->start(10); 

    // Création d'un timer pour les demandes spécifiques à l'accéléromètre
    accelTimer = new QTimer(this);
    connect(accelTimer, &QTimer::timeout, this, &MainWindow::onAccelTimer_Tick);
    accelTimer->start(200);  

    // Timer pour envoyer des requêtes aux cartes ID_1 et ID_3
    refreshTimer = new QTimer(this);
    connect(refreshTimer, &QTimer::timeout, this, &MainWindow::onRefresh_Tick);
    refreshTimer->start(40); 
}


void MainWindow::on_autoToggleButton_clicked(){
    scpp::CanFrame frame_to_write;
    if(ui->autoToggleButton->text()=="Auto"){
        ui->autoToggleButton->setText("Manuel");
    }else {
        ui->autoToggleButton->setText("Auto");
    }
    frame_to_write.id = 0x3; 
    frame_to_write.len = 1;
    frame_to_write.data[0] = 0x50; 
    frame_to_write.len  = 8;  
    for (int i = 1; i < 8; i++) {
        frame_to_write.data[i] = 0;
    }
    socket_can.write(frame_to_write); 
    // Send CAN frame
    //std::cout << "mode auto active" << std::endl;
}
