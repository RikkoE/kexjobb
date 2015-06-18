/****************************************************************************
**
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtAndroidExtras module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia. For licensing terms and
** conditions see http://qt.digia.com/licensing. For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights. These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

package org.qtproject.example.notification;

import android.content.Context;
import android.bluetooth.BluetoothAdapter;
import android.content.Intent;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.IntentFilter;
import android.util.Log;
import org.qtproject.qt5.android.bindings.QtActivity;

import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattDescriptor;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothProfile;

import java.util.UUID;
import java.util.Arrays;
import android.os.Handler;
import java.util.List;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedList;


/**
 * \brief The NotificationClient class
 * This class contains all the necessary code to connect to a BLE device,
 * fetch the characteristics, connect to the characteristics and convert
 * the data from the characteristics.
 *
 * @author Rickard Eriksson
 * @author Sajjadali Hemani
 */
public class NotificationClient extends QtActivity
{
    // Makes an instance of this class
    private static NotificationClient m_instance;

    // Sets the time for scanning in milliseconds
    private static final long SCAN_PERIOD = 10000;

    // Keeps the connected GATT server
    private BluetoothGatt mBluetoothGatt;

    /**
     * These UUIDs are the unique identifiers that lets the program decide what to
     * do with the data that is recieved, how the connection should be done
     * and what to display to the user.
     */
    private static final UUID HEART_RATE_SERVICE = UUID.fromString("0000180d-0000-1000-8000-00805f9b34fb");
    private static final UUID HEART_RATE_MEASUREMENT = UUID.fromString("00002a37-0000-1000-8000-00805f9b34fb");

    private static final UUID BATTERY_LEVEL_SERVICE = UUID.fromString("0000180f-0000-1000-8000-00805f9b34fb");
    private static final UUID BATTERY_LEVEL_INDICATOR = UUID.fromString("00002a19-0000-1000-8000-00805f9b34fb");

    private static final UUID DEVICE_INFORMATION_SERVICE = UUID.fromString("0000180a-0000-1000-8000-00805f9b34fb");
    private static final UUID DEVICE_NAME_STRING = UUID.fromString("00002a29-0000-1000-8000-00805f9b34fb");

    private static final UUID ECG_SERVICE = UUID.fromString("4E1B94B0-81AA-11E2-9E96-0800200C9A66");
    private static final UUID ECG_MEASUREMENT_CHARACTERISTIC = UUID.fromString("4E1B94B1-81AA-11E2-9E96-0800200C9A66");
    private static final UUID ECG_SAMPLING_RATE_REF_VOLTAGE = UUID.fromString("4E1B94B2-81AA-11E2-9E96-0800200C9A66");
    private static final UUID ECG_WRITE_CHARACTERISTIC = UUID.fromString("4E1B94B3-81AA-11E2-9E96-0800200C9A66");

    private static final UUID CLIENT_CHARACTERISTIC_CONFIG = UUID.fromString("00002902-0000-1000-8000-00805f9b34fb");

    // Keeps the heart rate in BPM
    private static int heartRateVal = 0;
    // The global variable that holds the reference voltage of the ECG
    private static int glob_refVoltage = 0;
    // The global variable that holds the sampling rate of the ECG
    private static int glob_samplingRate = 0;
    // The global variable that holds the battery level of the remote device
    private static int glob_batteryLevel = 0;

    // The global variable that holds the manufacturer name of the remote device
    private static String glob_manufacturerName;
    // An array that keeps the latest list of services of the remote device
    private static String[] servicesList;

    // This is used to get to the functions of the local bluetooth module
    private BluetoothAdapter bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();

    // A list to keep the names of known characteristics
    private static List<String> knownServices = new ArrayList<String>();

    // A list acting as a queue for the ECG data samples
    private static LinkedList<int[]> ecgDataQueue = new LinkedList<int[]>();

    /* HashMap to hold the devices names and addresses connected to eachother
     * The KEY is the address of the device, the VALUE is the name of the device
     */
    private static HashMap<String, String> devicesInformation = new HashMap();

    /* The array that holds the names and addresses of the devices to
     * send to the main application. First is the address followed by the name.
     */
    private static String[] sendList;
    // This is used to see the state when connecting
    private static boolean doneConnecting = false;
    // This is used to check if the ECG notification is on or not
    private static boolean ecgNotifyDisabled = true;
    // This is used to wait for initial data from the ECG characteristic
    private static boolean waitForData = true;
    // This is used to block the ECG queue so it can't use the list in two places at once
    private static boolean notReady = false;

    // A handler to start the timer for scanning to shut down
    private static Handler mHandler = new Handler();
    // This is used to see the status of scanning
    private static boolean scanning = true;

    // Makes an instance of itself
    public NotificationClient()
    {
        m_instance = this;
    }

    /**
     * \brief
     * Turns the bluetooth on
     */
    public static void btON() {
        System.out.println(m_instance.bluetoothAdapter.isEnabled());
        // Checks if the bluetooth module is on
        if (!m_instance.bluetoothAdapter.isEnabled()) {
            System.out.println("Turning bluetooth on");
            // Sends a request to the screen so the user can decide to allow bluetooth or not
            m_instance.startActivityForResult(new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE), 0);
        }
    }

    /**
     * \brief
     * Turns the bluetooth off
     */
    public static void btOFF() {
        System.out.println("Turning bluetooth off");
        // Turns off the bluetooth module
        m_instance.bluetoothAdapter.disable();
    }

    /**
     * \brief
     * Disconnect the device currently connected
     */
    public static void disconnectDevice() {
        // Disconnects the bluetooth low energy device
        m_instance.mBluetoothGatt.disconnect();
        // Resets the BluetoothGatt to be able to connect to a new device
        m_instance.mBluetoothGatt = null;
        // Clears the list of services for the device
        m_instance.knownServices.clear();
    }

    /**
     * \brief
     * Connects to a device given the address of that device
     *
     * @param address is the address of the device the user
     * wants to connect to.
     */
    public static void connectDevice(String address) {
        int i;
        /* Sets the state of the connection to false to block calls to
         * updateServices from returning until done */
        doneConnecting = false;

        final BluetoothDevice device = m_instance.bluetoothAdapter.getRemoteDevice(address);
        /* Calls the method that connects the host device to the ble device.
         * The first parameter is the Context, the second sets autoconnect to
         * false and the third connects a callback for the connection */
        m_instance.mBluetoothGatt = device.connectGatt(m_instance, false, m_instance.mGattCallback);
    }

    /**
     * \brief
     * Updates the list of available services/characteristic of
     * the connected device.
     * @return servicesList is the String array containing the
     * updated version of the available services.
     */
    public static String[] updateServices() {
        // Locks the application to wait for the connection to be established
        while(!doneConnecting);
        return servicesList;
    }

    /**
     * \brief
     * Disconnects the characteristic currently connected. If it is
     * a notification characteristic, the notification is turned off
     * and any special shut off can be applied here.
     * @param characteristic is the string containing the name of the
     * characteristic to be disconnected.
     */
    public static void disconnectNotification(String characteristic) {
        System.out.println("DISCONNECTING SERVICE");
        if(characteristic.equals("ECG measurement")) {
            // Gets the characteristic to turn measurement on/off
            BluetoothGattCharacteristic turnOffMeasure = m_instance.mBluetoothGatt.getService(ECG_SERVICE).getCharacteristic(ECG_WRITE_CHARACTERISTIC);
            // Creates a byte array to turn on measurement of ECG
            byte[] value = {(byte) 0x11};
            // Sets the local value of the characteristic to 11
            turnOffMeasure.setValue(value);
            // Sets the remote value of the characteristic to 11
            m_instance.mBluetoothGatt.writeCharacteristic(turnOffMeasure);
            // Change value to show notification has been disabled
            ecgNotifyDisabled = true;
        } else {
            System.out.println("Not a notification characteristic");
        }
    }

    /**
     * \brief
     * Uses the function readCharacteristic to fetch the battery
     * status of the remote device and returns it to the calling
     * function.
     * @return glob_batteryLevel is an integer containing the battery
     * level of the remote device.
     */
    public static int batteryLevel() {
        // Gets the characteristic for the battery level
        BluetoothGattCharacteristic batteryCharac = m_instance.mBluetoothGatt.getService(BATTERY_LEVEL_SERVICE).getCharacteristic(BATTERY_LEVEL_INDICATOR);
        // Calls the method that sends a read request to the remote device
        m_instance.readCharacteristic(batteryCharac);

        return glob_batteryLevel;
    }

    /**
     * \brief
     * Uses the function readCharacteristic to fetch the manufacturer
     * name of the remote device and
     * @return glob_manufacturerName is a String containing
     * the name of the manufacturer for the remote device.
     */
    public static String manufacName() {
        // Gets the characteristic for manufacturing name
        BluetoothGattCharacteristic manufacCharac = m_instance.mBluetoothGatt.getService(DEVICE_INFORMATION_SERVICE).getCharacteristic(DEVICE_NAME_STRING);
        // Calls the method that sends a read request to the remote device
        m_instance.readCharacteristic(manufacCharac);

        return glob_manufacturerName;
    }

    /**
     * \brief
     * Enables the notification for ECG on the remote and local device
     * if it is off and returns the data.
     * @return glob_ecgDataArray is an integer array with element 0
     * as the ecg timestamp and 1-6 are the samples from the remote
     * device.
     */
    public static int[] getEcgData() {
        // This part sets up the notification and runs only once
        if(ecgNotifyDisabled) {
            // Gets the characteristic for ECG measurement
            BluetoothGattCharacteristic ecgMeasure = m_instance.mBluetoothGatt.getService(ECG_SERVICE).getCharacteristic(ECG_MEASUREMENT_CHARACTERISTIC);
            // Calls for the method that activates the measurement and notification
            m_instance.setNotify(ecgMeasure);
            // Changes state so the program knows that notification is on
            ecgNotifyDisabled = false;
            // Locks the program until data is available to send
            waitForData = true;
            while(waitForData);
        }

        int[] sendData = new int[7];
        System.out.println("LIST SIZE before remove: " + ecgDataQueue.size());

        // Pause if a element is put in the list
        while(notReady);
        // Taking the oldest sample in the queue for sending
        sendData = ecgDataQueue.getFirst();
        System.out.println("Gotten the first element");
        // Deletes the sample taken out of the queue
        ecgDataQueue.removeFirst();
        System.out.println("Removed the first element");

        System.out.println("sendData: " + sendData);
        System.out.println("data queue: " + ecgDataQueue);

        // Send the samples
        return sendData;
    }

    /**
     * \brief
     * Checks if there are any ECG samples in the queue
     *
     * @return false if there are no samples waiting in the queue,
     * returns true if there are samples in the queue.
     */
    public static boolean newEcgDataAvailable() {
        // Check the size of the list queue
        if(ecgDataQueue.size() == 0) {
            return false;
        } else {
            return true;
        }
    }


    /**
     * \brief
     * Turns on the notification on the local device as well as
     * the remote device by writing to the characteristic descriptor.
     * @param characteristic is the characteristic that should notify.
     */
    public void setNotify(BluetoothGattCharacteristic characteristic) {
        // Enable local notifications
        mBluetoothGatt.setCharacteristicNotification(characteristic, true);
        // Enable remote notifications
        BluetoothGattDescriptor desc = characteristic.getDescriptor(CLIENT_CHARACTERISTIC_CONFIG);
        desc.setValue(BluetoothGattDescriptor.ENABLE_NOTIFICATION_VALUE);
        // Writes to descriptor on remote device to enable notification
        mBluetoothGatt.writeDescriptor(desc);
    }

    /**
     * This is the callback for the connection and data exchange
     * with the remote Bluetooth low energy device. It contains
     * many different responses for when connection status changes
     * or data is written/recieved.
     *
     */
    private BluetoothGattCallback mGattCallback = new BluetoothGattCallback() {

        /**
         * This is invoked if the connection state changes and can be handled
         * accordingly.
         */
        @Override
        public void onConnectionStateChange(BluetoothGatt gatt, int status, int newState) {
            // Connection established
            if (status == BluetoothGatt.GATT_SUCCESS && newState == BluetoothProfile.STATE_CONNECTED) {
                System.out.println("Connected");
                // Discover services
                gatt.discoverServices();

            // Connection disconnected
            } else if (status == BluetoothGatt.GATT_SUCCESS && newState == BluetoothProfile.STATE_DISCONNECTED) {
                // Handle a disconnect event
                System.out.println("Disconnected");
            }
        }

        /**
         * This is invoked if a characteristic that has notification
         * activated changes and can be handled accordingly.
         */
        @Override
        public void onCharacteristicChanged(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic) {
            if(HEART_RATE_MEASUREMENT.equals(characteristic.getUuid())) {
                heartRateVal = characteristic.getIntValue(BluetoothGattCharacteristic.FORMAT_UINT8, 1);
            } else if(ECG_MEASUREMENT_CHARACTERISTIC.equals(characteristic.getUuid())) {
                byte[] bytes;
                // Turn the value from the characteristic to a byte array
                bytes = characteristic.getValue();

                int[] glob_ecgDataArray = new int[7];

                // The timestamp is sixteen bits and has to be converted accordingly
                glob_ecgDataArray[0] = getIntSixteen(bytes,0);

                int offset = 2;
                /* A loop that goes through all the samples and converts them to
                 * integers of 24 bits of value */
                for(int i = 1; i < glob_ecgDataArray.length; i++) {
                    glob_ecgDataArray[i] = getIntTwentyFour(bytes, offset);
                    offset += 3;
                }

                // Stop the program from deleting elements while it is adding elements
                notReady = true;
                // Adding the samples to the ECG sample queue
                ecgDataQueue.addLast(glob_ecgDataArray);

                // Release the program from the lock
                notReady = false;

                // This value is only relevant for the first notification
                waitForData = false;
            } else {
                System.out.println("Characteristic not found");
            }

        }

        /**
         * This is invoked if services are discovered on the device
         * that is connected.
         */
        @Override
        public void onServicesDiscovered(BluetoothGatt gatt, int status) {
            System.out.println("Service discovered");
            // Saves the available services in an array
            servicesList = getGattServices(gatt.getServices());
            // This line makes it so the application can reach the servicesList
            doneConnecting = true;
        }

        /**
         * This is invoked if the device have written to the remote
         * device. This is used for characteristics that needs to
         * be written before they can be used.
         */
        @Override
        public void onCharacteristicWrite(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic, int status) {
            System.out.println("on characteristic write");

            if(status == BluetoothGatt.GATT_SUCCESS) {
                System.out.println("Write successful");
            }
            if(HEART_RATE_MEASUREMENT.equals(characteristic.getUuid())) {
                readCharacteristic(characteristic);
            } else if(ECG_WRITE_CHARACTERISTIC.equals(characteristic.getUuid())) {
                System.out.println("ECG charac written!");
            }
        }

        /**
         * This is invoked if a descriptor is written on the remote
         * device. This is used for characteristics that needs both
         * descriptor and another characteristic to be written.
         */
        @Override
        public void onDescriptorWrite(BluetoothGatt gatt, BluetoothGattDescriptor descriptor, int status) {
            if(status == BluetoothGatt.GATT_SUCCESS) {
                System.out.println("Descriptor status is: "+status);
                // Gets the characteristic that turns ECG measurement on/off
                BluetoothGattCharacteristic turnOnMeasure = m_instance.mBluetoothGatt.getService(ECG_SERVICE).getCharacteristic(ECG_WRITE_CHARACTERISTIC);
                // An array that is used to turn off the measurement
                byte[] value = {(byte) 0x01};
                // Turn measurement off on the local device
                turnOnMeasure.setValue(value);
                // Turn measurement off on the remote device
                m_instance.mBluetoothGatt.writeCharacteristic(turnOnMeasure);
            }
        }

        /**
         * This is invoked if a characteristic on the remote device
         * has been read and is handled accordingly, sometimes with
         * bitshifting.
         */
        @Override
        public void onCharacteristicRead(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic, int status) {

            // A read request was sent from the battery level characteristic
            if(status == BluetoothGatt.GATT_SUCCESS && BATTERY_LEVEL_INDICATOR.equals(characteristic.getUuid())) {
                int format = -1;

                // Decides that the format on the int for battery level is uint 8 (1 byte)
                format = BluetoothGattCharacteristic.FORMAT_UINT8;

                // Takes out the integer battery level from the read value
                final int batteryLevel = characteristic.getIntValue(format, 0);

                // Sets the global battery level to the same as the read value
                glob_batteryLevel = batteryLevel;
                System.out.println("Battery level: " + batteryLevel);

            // A read request was sent from the manufacturer name characteristic
            } else if(status == BluetoothGatt.GATT_SUCCESS && DEVICE_NAME_STRING.equals(characteristic.getUuid())) {
                // Takes out the String from the read value
                String manufacturer = characteristic.getStringValue(0);

                // Sets the global manufacturer name to the same as the read value
                glob_manufacturerName = manufacturer;
                System.out.println("Manufacturer: " + manufacturer);

            // A read request was sent from the ECG sampling rate and reference voltage characteristic
            } else if(status == BluetoothGatt.GATT_SUCCESS && ECG_SAMPLING_RATE_REF_VOLTAGE.equals(characteristic.getUuid())) {
                int format = -1;

                // Decides that the format on the int for reference voltage is uint 8 (1 byte)
                format = BluetoothGattCharacteristic.FORMAT_UINT8;

                // Takes out the integer reference voltage from the read value
                final int refVoltage = characteristic.getIntValue(format, 0);
                // Sets the global reference voltage to the same as the read value
                glob_refVoltage = refVoltage;

//                format = BluetoothGattCharacteristic.FORMAT_UINT16;
//                final int samplingRate = characteristic.getIntValue(format, 1);

//                glob_samplingRate = samplingRate;

                System.out.println("Ref voltage: " + refVoltage);
//                System.out.println("Sampling rate: " + samplingRate);
            } else {
                System.out.println("Read unsuccessful");
            }

        }

    };

    /**
     * \brief
     * This method shifts two bytes (16 bits) in a byte array and
     * converts the result to an integer.
     * @param array is the byte array to be turned into an integer.
     * @param offset is the offset to where the convertion should
     * start in the array.
     * @return the integer of the combined two bytes.
     */
    public int getIntSixteen(byte[] array, int offset) {
        return
          ((array[offset] & 0xff) << 8) |
           (array[offset+1] & 0xff);
    }

    /**
     * \brief
     * This method shifts three bytes (24 bits) in a byte array and
     * converts the result to an integer.
     * @param array is the byte array to be turned into an integer.
     * @param offset is the offset to where the convertion should
     * start in the array.
     * @return the integer of the combined three bytes.
     */
    public int getIntTwentyFour(byte[] array, int offset) {
        return
          ((array[offset] & 0xff) << 16) |
          ((array[offset+1] & 0xff) << 8) |
           (array[offset+2] & 0xff);
    }

    /**
     * \brief
     * Reads the desired characteristic of the remote device.
     * @param characteristic is the characteristic the user
     * wants to read from.
     */
    public void readCharacteristic(BluetoothGattCharacteristic characteristic) {
        if (bluetoothAdapter == null || mBluetoothGatt == null) {
//            Log.w(TAG, "BluetoothAdapter not initialized");
            return;
        }
        // Sends a read request of the specific characteristic
        mBluetoothGatt.readCharacteristic(characteristic);
    }

    /**
     * \brief
     * This method starts the scanning for remote devices
     * if the bluetooth module is on, otherwise it sends a
     * request to the method "btON". It also sets a handler
     * to stop the scanning after a predetermined time and
     * clears the list containing old information.
     */
    public static void scanLeDevices() {
        if(m_instance.bluetoothAdapter.isEnabled()) {
            // Clears the array of devices
            sendList = null;
            // Clears the HashMap of devices
            devicesInformation.clear();
            // Sets the scanning status to true
            scanning = true;
            /* Adds a handler to be activated after the time
             * specified in "SCAN_PERIOD" has run out */
            mHandler.postDelayed(new Runnable() {
                @Override
                public void run() {
                    System.out.println("Stopping scan");
                    // Stops the scanning connected to the specified callback
                    m_instance.bluetoothAdapter.stopLeScan(m_instance.mLeScanCallback);
                    // Sets scanning status to false
                    scanning = false;
                }
            }, SCAN_PERIOD);
            System.out.println("Start scanning");
            // Starts the scanning and connects to a scan callback
            m_instance.bluetoothAdapter.startLeScan(m_instance.mLeScanCallback);
        } else {
            // Sets scanning status to false
            scanning = false;
            // Calls the mehtod to turn on the bluetooth module
            btON();
        }
    }

    /**
     * \brief
     * Takes the list containing the devices found during the last scan,
     * converts it to an array and sends it to the caller.
     * @return a list of remote devices found as a String array.
     */
    public static String[] getDeviceList() {

        // Temporary list to hold the information about devices
        List<String> sendToCaller = new ArrayList<String>();

        // Loop through all the device names and addresses
        for (String key: devicesInformation.keySet()) {
            // Add the address to the list
            sendToCaller.add(key);
            // Add the name to the list
            sendToCaller.add(devicesInformation.get(key));
            System.out.println("Device address : " + key);
            System.out.println("Device name : " + devicesInformation.get(key));
        }

        // Convert the list to a String array
        sendList = sendToCaller.toArray(new String[sendToCaller.size()]);
        return sendList;
    }

    /**
     * \brief
     * Fetches the scanning status.
     * @return the result of the variable containing the status
     * of the scanning process.
     */
    public static boolean scanningStatus() {
        // returns true if scanning and false if not
        return scanning;
    }

    /**
     * This is the callback for the scanning of bluetooth low energy
     * devices. There are various methods that can be invoked inside
     * the callback but here only the onLeScan is used.
     */
    private BluetoothAdapter.LeScanCallback mLeScanCallback = new BluetoothAdapter.LeScanCallback() {

        /**
         * This is invoked if a remote bluetooth low energy device
         * has been found. If one has been found, the name and the
         * address of that device is added to a hashmap.
         */
        @Override
        public void onLeScan(final BluetoothDevice device, int rssi, byte[] scanRecord) {

            // Add the device name as the VALUE of the hashmap and the address as the KEY
            devicesInformation.put(device.getAddress(), device.getName());

            System.out.println("Device name: " + device.getName());
            System.out.println("Device address: " + device.getAddress());
            System.out.println("Rssi: " + rssi);
        }
    };

    /**
     * \brief
     * Cross-references the UUID with known characteristic UUIDs to
     * determine if the remote device has any usable characteristics.
     * @param uuid is the UUID of a characteristic to be examined.
     * @return the name of the characteristic.
     */
    private String getServiceName(String uuid) {
        if(uuid.equals(HEART_RATE_SERVICE.toString())) {
            return "Heart rate service";
        } else if(uuid.equals(BATTERY_LEVEL_SERVICE.toString())) {
            return "Battery level service";
        } else if(uuid.equals(HEART_RATE_MEASUREMENT.toString())) {
            return "Heart rate measurement";
        } else if(uuid.equals(BATTERY_LEVEL_INDICATOR.toString())) {
            return "Battery level indicator";
        } else if(uuid.equals(DEVICE_NAME_STRING.toString())) {
            return "Manufacturer name";
        } else if(uuid.equals(ECG_MEASUREMENT_CHARACTERISTIC.toString())) {
            return "ECG measurement";
        } else {
            return "null";
        }
    }

    /**
     * \brief
     * Takes a list of services and fetches their characteristics
     * as well as cross-referencing them to known characteristics.
     * @param gattServices is a list of BluetoothGattService to examine
     * what characteristics they have.
     * @return list of the names of the known characteristics.
     */
    private String[] getGattServices(List<BluetoothGattService> gattServices) {
        String uuid = null;
        // A list to temporarily hold the list of characteristics and services
        List<String> servicesAndCharacteristics = new ArrayList<String>();

        // Loops through all services found on the remote device
        for (BluetoothGattService gattService : gattServices) {
            // Converts the UUID to a string
            uuid = gattService.getUuid().toString();
            // Add the UUID to the list
            servicesAndCharacteristics.add(uuid);
            System.out.println("Service uuid: " + uuid);

            List<BluetoothGattCharacteristic> gattCharacteristics = gattService.getCharacteristics();
            // Loops through all the characteristics of a service on the remote device
            for(BluetoothGattCharacteristic gattCharac : gattCharacteristics) {
                // Converts the UUID to a string
                uuid = gattCharac.getUuid().toString();
                // Add the UUID to the list
                servicesAndCharacteristics.add(uuid);
                System.out.println("\t" + "Characteristic uuid: " + uuid);
                // Check if the UUID is a known characteristic
                String result = getServiceName(uuid);
                if(!result.equals("null")) {
                    System.out.println("known characteristic found: " + result);
                    // A known characteristic is found and added to the list of known services
                    m_instance.knownServices.add(result);
                }
            }
        }

        System.out.println("List: " + knownServices);

        /* The list of all known characteristics is converted to an array
         * and returned to the calling function */
        String[] sendKnown = new String[knownServices.size()];
        sendKnown = knownServices.toArray(sendKnown);

        return sendKnown;
    }
}
