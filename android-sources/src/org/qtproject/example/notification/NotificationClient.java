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

import android.app.Notification;
import android.app.NotificationManager;
import android.content.Context;
import android.bluetooth.BluetoothAdapter;
import android.content.Intent;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.IntentFilter;
import android.widget.ArrayAdapter;
import android.widget.Toast;
import android.util.Log;

import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattDescriptor;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothManager;
import android.bluetooth.BluetoothProfile;

import java.util.UUID;
import java.util.Arrays;
import android.os.Handler;
import java.util.List;
import java.util.ArrayList;



public class NotificationClient extends org.qtproject.qt5.android.bindings.QtActivity
{
    private static NotificationClient m_instance;

    // Stops scanning after 10 seconds.
    private static final long SCAN_PERIOD = 10000;

    private BluetoothGatt mBluetoothGatt;

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

    private static int heartRateVal = 0;
    private static int glob_ecgTimeStamp = 0;
    private static int glob_refVoltage = 0;
    private static int glob_samplingRate = 0;
    private static int glob_batteryLevel = 0;
    private static int[] glob_ecgDataArray = new int[7];
    private static String glob_manufacturerName;
    private static String[] servicesList;

    private BluetoothAdapter bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();

    private static List<String> devices = new ArrayList<String>();
    private static List<String> addresses = new ArrayList<String>();
    private static List<String> knownServices = new ArrayList<String>();

    private static String[] sendList;
    private static boolean doneConnecting = false;
    private static boolean notReadyToSend = false;
    private static boolean ecgNotifyDisabled = true;
    private static boolean waitForData = true;

    private static Handler mHandler = new Handler();
    private static boolean scanning = true;

    private static final String TAG = "Notification client";

    public NotificationClient()
    {
        m_instance = this;
    }

    public static void btON() {
        System.out.println(m_instance.bluetoothAdapter.isEnabled());
        // Checks if the bluetooth module is on
        if (!m_instance.bluetoothAdapter.isEnabled()) {
            System.out.println("Turning bluetooth on");
            // Sends a request to the screen so the user can decide to allow bluetooth or not
            m_instance.startActivityForResult(new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE), 0);
        }
    }

    public static void btOFF() {
        System.out.println("Turning bluetooth off");
        // Turns off the bluetooth module
        m_instance.bluetoothAdapter.disable();
    }

    public static void disconnectDevice() {
        // Disconnects the bluetooth low energy device
        m_instance.mBluetoothGatt.disconnect();
        // Resets the BluetoothGatt to be able to connect to a new device
        m_instance.mBluetoothGatt = null;
        // Clears the list of services for the device
        m_instance.knownServices.clear();
    }

    public static void connectDevice(String deviceName) {
        int i;
        /* Sets the state of the connection to false to block calls to
         * updateServices from returning until done */
        doneConnecting = false;
        String deviceAddress = "";

        // Gets the index of the chosen device
        int index = devices.indexOf(deviceName);
        // Gets the address (UUID) of the device
        deviceAddress = addresses.get(index);

        System.out.println("Connecting to " + deviceName + " with address " + deviceAddress);

        //
        final BluetoothDevice device = m_instance.bluetoothAdapter.getRemoteDevice(deviceAddress);
        /* Calls the method that connects the host device to the ble device.
         * The first parameter is the Context, the second sets autoconnect to
         * false and the third connects a callback for the connection */
        m_instance.mBluetoothGatt = device.connectGatt(m_instance, false, m_instance.mGattCallback);
    }

    public static String[] updateServices() {
        // Locks the application to wait for the connection to be established
        while(!doneConnecting);
        return servicesList;
    }

    public static void disconnectNotification(String characteristic) {
        System.out.println("DISCONNECTING SERVICE");
        if(characteristic.equals("ECG measurement")) {
            BluetoothGattCharacteristic turnOffMeasure = m_instance.mBluetoothGatt.getService(ECG_SERVICE).getCharacteristic(ECG_WRITE_CHARACTERISTIC);
            byte[] value = {(byte) 0x11};
            turnOffMeasure.setValue(value);
            m_instance.mBluetoothGatt.writeCharacteristic(turnOffMeasure);
            ecgNotifyDisabled = true;
        } else {
            System.out.println("Not a notification characteristic");
        }
    }

    public static int batteryLevel() {
        BluetoothGattCharacteristic batteryCharac = m_instance.mBluetoothGatt.getService(BATTERY_LEVEL_SERVICE).getCharacteristic(BATTERY_LEVEL_INDICATOR);
        m_instance.readCharacteristic(batteryCharac);

        return glob_batteryLevel;
    }

    public static String manufacName() {
        BluetoothGattCharacteristic manufacCharac = m_instance.mBluetoothGatt.getService(DEVICE_INFORMATION_SERVICE).getCharacteristic(DEVICE_NAME_STRING);
        m_instance.readCharacteristic(manufacCharac);

        return glob_manufacturerName;
    }

    public static int[] getEcgData() {
        if(ecgNotifyDisabled) {
            BluetoothGattCharacteristic ecgMeasure = m_instance.mBluetoothGatt.getService(ECG_SERVICE).getCharacteristic(ECG_MEASUREMENT_CHARACTERISTIC);
            m_instance.setNotify(ecgMeasure);

            ecgNotifyDisabled = false;
            waitForData = true;
            while(waitForData);
        }
        return glob_ecgDataArray;
    }


    public void setNotify(BluetoothGattCharacteristic characteristic) {
        // Enable local notifications
        mBluetoothGatt.setCharacteristicNotification(characteristic, true);
        // Enable remote notifications
        BluetoothGattDescriptor desc = characteristic.getDescriptor(CLIENT_CHARACTERISTIC_CONFIG);
        desc.setValue(BluetoothGattDescriptor.ENABLE_NOTIFICATION_VALUE);
        // Writes to descriptor on remote device to enable notification
        mBluetoothGatt.writeDescriptor(desc);
    }

    private BluetoothGattCallback mGattCallback = new BluetoothGattCallback() {
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

        @Override
        public void onCharacteristicChanged(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic) {
//            System.out.println("Characteristic changed" + characteristic);
            if(HEART_RATE_MEASUREMENT.equals(characteristic.getUuid())) {
                heartRateVal = characteristic.getIntValue(BluetoothGattCharacteristic.FORMAT_UINT8, 1);
            } else if(ECG_MEASUREMENT_CHARACTERISTIC.equals(characteristic.getUuid())) {
                byte[] bytes;
                // Turn the value from the characteristic to a byte array
                bytes = characteristic.getValue();
                // The timestamp is sixteen bits and has to be converted accordingly
                glob_ecgDataArray[0] = getIntSixteen(bytes,0);

                int offset = 2;
                /* A loop that goes through all the samples and converts them to
                 * integers of 24 bits of value */
                for(int i = 1; i < glob_ecgDataArray.length; i++) {
                    glob_ecgDataArray[i] = getIntTwentyFour(bytes, offset);
                    offset += 3;
                }
                // This value is only relevant for the first notification
                waitForData = false;
            } else {
                System.out.println("Characteristic not found");
            }

        }

        @Override
        public void onServicesDiscovered(BluetoothGatt gatt, int status) {
            System.out.println("Service discovered");
            // Saves the available services in an array
            servicesList = getGattServices(gatt.getServices());
            // This line makes it so the application can reach the servicesList
            doneConnecting = true;
        }

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

        @Override
        public void onDescriptorWrite(BluetoothGatt gatt, BluetoothGattDescriptor descriptor, int status) {
            if(status == BluetoothGatt.GATT_SUCCESS) {
                System.out.println("Descriptor status is: "+status);
                BluetoothGattCharacteristic turnOnMeasure = m_instance.mBluetoothGatt.getService(ECG_SERVICE).getCharacteristic(ECG_WRITE_CHARACTERISTIC);
                byte[] value = {(byte) 0x01};

                turnOnMeasure.setValue(value);
                m_instance.mBluetoothGatt.writeCharacteristic(turnOnMeasure);
            }
        }

        @Override
        public void onCharacteristicRead(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic, int status) {

            if(status == BluetoothGatt.GATT_SUCCESS && BATTERY_LEVEL_INDICATOR.equals(characteristic.getUuid())) {
                int flag = characteristic.getProperties();
                int format = -1;
                format = BluetoothGattCharacteristic.FORMAT_UINT8;
                final int batteryLevel = characteristic.getIntValue(format, 0);
                glob_batteryLevel = batteryLevel;
                System.out.println("Battery level: " + batteryLevel);

                notReadyToSend = false;
            } else if(status == BluetoothGatt.GATT_SUCCESS && DEVICE_NAME_STRING.equals(characteristic.getUuid())) {
                String manufacturer = characteristic.getStringValue(0);
                glob_manufacturerName = manufacturer;
                System.out.println("Manufacturer: " + manufacturer);

                notReadyToSend = false;
            } else if(status == BluetoothGatt.GATT_SUCCESS && ECG_SAMPLING_RATE_REF_VOLTAGE.equals(characteristic.getUuid())) {
                int format = -1;
                format = BluetoothGattCharacteristic.FORMAT_UINT8;
                final int refVoltage = characteristic.getIntValue(format, 0);
                glob_refVoltage = refVoltage;

                format = BluetoothGattCharacteristic.FORMAT_UINT16;
                final int samplingRate = characteristic.getIntValue(format, 1);
                glob_samplingRate = samplingRate;

                System.out.println("Ref voltage: " + refVoltage);
                System.out.println("Sampling rate: " + samplingRate);
            } else {
                System.out.println("Read unsuccessful");
            }

        }

    };

    public int getIntSixteen(byte[] array, int offset) {
        return
          ((array[offset] & 0xff) << 8) |
           (array[offset+1] & 0xff);
    }

    public int getIntTwentyFour(byte[] array, int offset) {
        return
          ((array[offset] & 0xff) << 16) |
          ((array[offset+1] & 0xff) << 8) |
           (array[offset+2] & 0xff);
    }

    public void readCharacteristic(BluetoothGattCharacteristic characteristic) {
        if (bluetoothAdapter == null || mBluetoothGatt == null) {
            Log.w(TAG, "BluetoothAdapter not initialized");
            return;
        }
        mBluetoothGatt.readCharacteristic(characteristic);
    }

    public static void scanLeDevices() {
        if(m_instance.bluetoothAdapter.isEnabled()) {
            sendList = null;
            devices.clear();
            addresses.clear();
            scanning = true;
            mHandler.postDelayed(new Runnable() {
                @Override
                public void run() {
                    System.out.println("Stopping scan");
                    m_instance.bluetoothAdapter.stopLeScan(m_instance.mLeScanCallback);
                    scanning = false;
                }
            }, SCAN_PERIOD);
            System.out.println("Start scanning");
            m_instance.bluetoothAdapter.startLeScan(m_instance.mLeScanCallback);
        } else {
            scanning = false;
            btON();
        }
    }

    public static String[] getDeviceList() {
        sendList = devices.toArray(new String[devices.size()]);
        return sendList;
    }

    public static boolean scanningStatus() {
        return scanning;
    }

    private BluetoothAdapter.LeScanCallback mLeScanCallback = new BluetoothAdapter.LeScanCallback() {

        @Override
        public void onLeScan(final BluetoothDevice device, int rssi, byte[] scanRecord) {
            int length = 0;

            devices.add(device.getName());
            addresses.add(device.getAddress());

            System.out.println("Device name: " + device.getName());
            System.out.println("Device address: " + device.getAddress());
            System.out.println("Rssi: " + rssi);
        }
    };

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

    private String[] getGattServices(List<BluetoothGattService> gattServices) {
        System.out.println(gattServices);
        String uuid = null;
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
