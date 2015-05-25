#include "jnihealthyway.h"


///
/// \brief
/// The default constructor.
///
JNIHealthyWay::JNIHealthyWay()
{

}

///
/// \brief
/// The default deconstructor
///
JNIHealthyWay::~JNIHealthyWay()
{

}

///
/// \brief
/// Function that takes the name of a device and
/// tries to connect to it by sending that name to
/// the java function "connectDevice".
///
/// \param deviceName takes the device name to connect too as a QString.
///
void JNIHealthyWay::connectDevice(QString deviceName)
{
    // Converts the QString to a QAndroidObject that can be converted to a jstring
    QAndroidJniObject deviceString = QAndroidJniObject::fromString(deviceName);
    QAndroidJniObject::callStaticMethod<void>("org/qtproject/example/notification/NotificationClient",
                                              "connectDevice",
                                              "(Ljava/lang/String;)V",
                                              deviceString.object<jstring>());
}

///
/// \brief
/// Disconnects from a device through calling a java
/// function "disconnectDevice".
///
void JNIHealthyWay::disconnectDevice()
{
    QAndroidJniObject::callStaticMethod<void>("org/qtproject/example/notification/NotificationClient",
                                              "disconnectDevice");
}

///
/// \brief JNIHealthyWay::scanForDevices
/// Starts to scan for Bluetooth low energy devices through
/// calling the java function "scanLeDevices".
///
void JNIHealthyWay::scanForDevices()
{
    QAndroidJniObject::callStaticMethod<void>("org/qtproject/example/notification/NotificationClient",
                                              "scanLeDevices");
}

///
/// \brief
/// A function that disconnects from the data stream of the chosen
/// characteristic.
///
/// \param characteristic takes a QString with the
/// name of the characteristic the user wants to disconnect from.
///
void JNIHealthyWay::disconnectDataStream(QString characteristic)
{
    qDebug() << "Disconnect service: " << characteristic;

    // Converts the QString to a QAndroidObject that can be converted to a jstring
    QAndroidJniObject javaString = QAndroidJniObject::fromString(characteristic);
    QAndroidJniObject::callStaticMethod<void>("org/qtproject/example/notification/NotificationClient",
                                              "disconnectNotification",
                                              "(Ljava/lang/String;)V",
                                              javaString.object<jstring>());
}

///
/// \brief
/// A function that simply tells the device to turn off its'
/// bluetooth module.
///
void JNIHealthyWay::turnBluetoothOff()
{
    QAndroidJniObject::callStaticMethod<void>("org/qtproject/example/notification/NotificationClient",
                                              "btOFF");
}

///
/// \brief
/// A function that tells the device to turn on its' bluetooth
/// module.
///
void JNIHealthyWay::turnBluetoothOn()
{
    QAndroidJniObject::callStaticMethod<void>("org/qtproject/example/notification/NotificationClient",
                                              "btON");
}

///
/// \brief
/// A function that checks the status of the device that is scanning
/// \return
/// a boolean of true if the bluetooth module is scanning
/// and false if the scanning is done.
///
bool JNIHealthyWay::scanningStatus()
{
    jboolean scanStatus = QAndroidJniObject::callStaticMethod<jboolean>("org/qtproject/example/notification/NotificationClient",
                                                                      "scanningStatus");
    /* A jboolean returns 0 for false and 1 for true,
     * this converts it into regular true/false */
    if(scanStatus == 0) {
       return false;
    } else {
       return true;
    }
}

///
/// \brief
/// A function that fetches the list of services discovered by
/// the device and constructs it into a QStringList that can be used
/// in the main program.
/// \return
/// a QStringList with the processed result containing
/// all the services available to the user.
///
QStringList JNIHealthyWay::listServices()
{
    QStringList listOfServices;
    QAndroidJniEnvironment env;
    jstring string;
    const char *formatted;

    QAndroidJniObject javaArray = QAndroidJniObject::callStaticObjectMethod("org/qtproject/example/notification/NotificationClient",
                                                                              "updateServices",
                                                                              "()[Ljava/lang/String;");

    // Converts the QAndroidJniObject into a jobjectArray
    jobjectArray servicesFromJava = javaArray.object<jobjectArray>();
    int size = env->GetArrayLength(servicesFromJava);

    /* Loop that converts all the elements in the jobjectArray
     * into QStrings and puts them in a QStringList*/
    for (int i=0; i < size; i++)
    {
        string = (jstring)env->GetObjectArrayElement(servicesFromJava, i);
        formatted = env->GetStringUTFChars(string, 0);
        // Puts the QString in the QStringList
        listOfServices.append(formatted);
        /* These two lines releases the variables from the memory
         * to prevent unnecessary usage of memory*/
        env->ReleaseStringUTFChars(string, formatted);
        env->DeleteLocalRef(string);
    }

    return listOfServices;
}

///
/// \brief
/// A function that fetches the list of BLE devices discovered by
/// the device and constructs it into a QStringList that can be used
/// in the main program.
/// \return
/// a QStringList with the processed result containing
/// all the devices available in the area.
///
QStringList JNIHealthyWay::listDevices()
{
    QStringList listOfDevices;
    QAndroidJniEnvironment env;
    jstring string;
    const char *formatted;

    QAndroidJniObject stringArray = QAndroidJniObject::callStaticObjectMethod("org/qtproject/example/notification/NotificationClient",
                                                                              "getDeviceList",
                                                                              "()[Ljava/lang/String;");

    // Converts the QAndroidJniObject into a jobjectArray
    jobjectArray arr = stringArray.object<jobjectArray>();
    int size = env->GetArrayLength(arr);

    /* Loop that converts all the elements in the jobjectArray
     * into QStrings and puts them in a QStringList*/
    for (int i=0; i < size; i++)
    {
        string = (jstring)env->GetObjectArrayElement(arr, i);
        formatted = env->GetStringUTFChars(string, 0);
        // Puts the QString in the QStringList
        listOfDevices.append(formatted);
        /* These two lines releases the variables from the memory
         * to prevent unnecessary usage of memory*/
        env->ReleaseStringUTFChars(string, formatted);
        env->DeleteLocalRef(string);
    }

    return listOfDevices;
}

///
/// \brief
/// Gets the battery level from the connected device by calling the
/// native function "batteryLevel".
/// \return
/// an integer representing the battery level of the connected device.
///
int JNIHealthyWay::getBatteryLevel()
{
    int batteryLevel;

    jint javaBatt = QAndroidJniObject::callStaticMethod<jint>("org/qtproject/example/notification/NotificationClient",
                                                       "batteryLevel");
    // Converts the jint into a regular int
    batteryLevel = (int) javaBatt;

    return batteryLevel;
}

///
/// \brief
/// Fetches the manufacturer name of the connected device by calling
/// the native function "manufacName".
/// \return
/// a QString containing the manufacturere name of the connected device.
///
QString JNIHealthyWay::getManufacturerName()
{
    QString manufacturerName;

    QAndroidJniObject fromJava = QAndroidJniObject::callStaticObjectMethod("org/qtproject/example/notification/NotificationClient",
                                                                           "manufacName",
                                                                           "()Ljava/lang/String;");
    // Converts the jstring into a QString
    manufacturerName = fromJava.toString();

    return manufacturerName;
}

///
/// \brief
/// Fetches the ECG data from the connected device by calling the native function
/// "getEcgData". The function then converts the result into an int array.
/// \return
/// a pointer to the array where the ECG data is stored, including timestamp
/// and ECG reading samples.
///
int *JNIHealthyWay::getEcgData()
{
    QAndroidJniEnvironment env;

    QAndroidJniObject javaArray = QAndroidJniObject::callStaticObjectMethod<jintArray>("org/qtproject/example/notification/NotificationClient",
                                                    "getEcgData");

    // Converts the QAndroidJniObject into a jintArray
    jintArray intArray = javaArray.object<jintArray>();

    int size = env->GetArrayLength(intArray);

    // Converts the jintArray into an array of ints
    jint *temp = env->GetIntArrayElements(intArray, 0);

    /* Loop that converts all the jints into regular integers and
     * puts them in the ecgData int array*/
    for (int i = 0; i < size; i++)
    {
        ecgData[i] = (int) temp[i];
    }

    // Releases the int array to free the memory
    env->ReleaseIntArrayElements(intArray, temp, 0);

    return ecgData;
}


