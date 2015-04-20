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


public class NotificationClient extends org.qtproject.qt5.android.bindings.QtActivity
{

    private static MyJavaNatives natives = new MyJavaNatives();

    private static NotificationClient m_instance;

    private static boolean mScanning;

    private static final int REQUEST_ENABLE_BT = 1;
    // Stops scanning after 10 seconds.
    private static final long SCAN_PERIOD = 10000;

//    private static native void callNativeFunction(int x);

    public NotificationClient()
    {
        m_instance = this;
    }

    private BluetoothAdapter bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
    private static String[] devicesFound = new String[20];
    private static String[] sendList;

    static int deviceNumber = 0;
    static int nrOfDevices = 0;

    private static boolean searching = true;

    BroadcastReceiver mReceiver = new BroadcastReceiver() {
                public void onReceive(Context context, Intent intent) {
                    String action = intent.getAction();

                    if (BluetoothAdapter.ACTION_DISCOVERY_STARTED.equals(action)) {
                        searching = true;
                        //discovery starts, we can show progress dialog or perform other tasks
                    } else if (BluetoothAdapter.ACTION_DISCOVERY_FINISHED.equals(action)) {
                        searching = false;
                        //discovery finishes, dismiss progress dialog
                    } else if (BluetoothDevice.ACTION_FOUND.equals(action)) {
                        //bluetooth device found
                        BluetoothDevice device = (BluetoothDevice) intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                        System.out.println("Found a device");
                        if(device.getName() == "null" || device.getName() == "") {
                        } else {
                            devicesFound[deviceNumber] = device.getAddress();
                            deviceNumber++;
                            nrOfDevices++;
                        }
                    }
                }
            };


    public static String[] scanReturn() {
        m_instance.bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();

        if(m_instance.bluetoothAdapter.isEnabled()) {

            IntentFilter filter = new IntentFilter();
            m_instance.bluetoothAdapter.startDiscovery();

            filter.addAction(BluetoothDevice.ACTION_FOUND);
            filter.addAction(BluetoothAdapter.ACTION_DISCOVERY_STARTED);
            filter.addAction(BluetoothAdapter.ACTION_DISCOVERY_FINISHED);

            m_instance.registerReceiver(m_instance.mReceiver, filter);

            while(searching);
            searching = true;
        }

            if(nrOfDevices > 0){
                sendList = new String[nrOfDevices];
                System.out.println("nrOfDevices: " + nrOfDevices);
                for(int i=0;i<nrOfDevices;i++) {
                    System.out.println("Device name: "+devicesFound[i]);
                    sendList[i] = devicesFound[i];
                }

                System.out.println("Device 0: " + sendList[0]);
            } else {
                sendList = new String[1];
                sendList[0] = "No devices";
            }
            nrOfDevices = 0;
            deviceNumber = 0;
            return sendList;
    }

    public static void btON() {
        System.out.println("asdf");
        System.out.println(m_instance.bluetoothAdapter.isEnabled());
        if (!m_instance.bluetoothAdapter.isEnabled()) {
            System.out.println("bluetooth is turned off");
//             m_instance.startActivityForResult(new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE), 0);
            m_instance.bluetoothAdapter.enable();
        }

    }
    public static void btOFF() {
        System.out.println("Bluetooth off");
        m_instance.bluetoothAdapter.disable();
    }


    private BluetoothGatt mBluetoothGatt;
    private static final UUID HEART_RATE_MEASUREMENT = UUID.fromString("00002a37-0000-1000-8000-00805f9b34fb");
    private static final UUID HEART_RATE_SERVICE = UUID.fromString("0000180d-0000-1000-8000-00805f9b34fb");
    private static final UUID CLIENT_CHARACTERISTIC_CONFIG = UUID.fromString("00002902-0000-1000-8000-00805f9b34fb");
    private static int heartRateVal = 0;
    private static UUID[] uuids = new UUID[1];


    public static void connectDevice(/*final String address*/) {
        final String NORDIC_ADDRESS = "F5:4C:19:63:13:7B";
        System.out.println("Connecting to " + NORDIC_ADDRESS);
        final BluetoothDevice device = m_instance.bluetoothAdapter.getRemoteDevice(NORDIC_ADDRESS);
        m_instance.mBluetoothGatt = device.connectGatt(m_instance, false, m_instance.mGattCallback);
    }

    public static int updateHeartRate() {
        String fromNative = natives.print();
        return heartRateVal;
    }

    private BluetoothGattCallback mGattCallback = new BluetoothGattCallback() {
        @Override
        public void onConnectionStateChange(BluetoothGatt gatt, int status, int newState) {
            //Connection established
            if (status == BluetoothGatt.GATT_SUCCESS
                    && newState == BluetoothProfile.STATE_CONNECTED) {
                System.out.println("Connected");
                //Discover services
                gatt.discoverServices();

            } else if (status == BluetoothGatt.GATT_SUCCESS
                    && newState == BluetoothProfile.STATE_DISCONNECTED) {
                System.out.println("Disconnected");
                //Handle a disconnect event

            }
        }

        private void enableNextSensor(BluetoothGatt gatt) {
            System.out.println("Enable next sensor");

            BluetoothGattCharacteristic characteristic;

            //Values to set determined by SensorTag docs
                    characteristic = gatt.getService(HEART_RATE_SERVICE)
                            .getCharacteristic(HEART_RATE_MEASUREMENT);
//                    characteristic.setValue(new byte[] {0x01});

            //Execute the write
            System.out.println("Write charac: " + gatt.writeCharacteristic(characteristic));
        }

        private void readNextSensor(BluetoothGatt gatt) {
            System.out.println("Read next sensor");

                    BluetoothGattCharacteristic characteristic;
                    characteristic = gatt.getService(HEART_RATE_SERVICE)
                            .getCharacteristic(HEART_RATE_MEASUREMENT);

            System.out.println("read charac: " + gatt.readCharacteristic(characteristic));
        }

        private void setNotifyNextSensor(BluetoothGatt gatt) {

            System.out.println("set notify next sensor");

                    BluetoothGattCharacteristic characteristic;

                    characteristic = gatt.getService(HEART_RATE_SERVICE)
                            .getCharacteristic(HEART_RATE_MEASUREMENT);

                    //Enable local notifications
                    gatt.setCharacteristicNotification(characteristic, true);
                    //Enabled remote notifications
                    BluetoothGattDescriptor desc = characteristic.getDescriptor(CLIENT_CHARACTERISTIC_CONFIG);
                    desc.setValue(BluetoothGattDescriptor.ENABLE_NOTIFICATION_VALUE);
                    gatt.writeDescriptor(desc);
                }

        public void onCharacteristicChanged(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic) {
            System.out.println("Characteristic changed" + characteristic);
            heartRateVal = characteristic.getIntValue(BluetoothGattCharacteristic.FORMAT_UINT8, 1);

            System.out.println("Data: " + heartRateVal);
        }


        @Override
        public void onServicesDiscovered(BluetoothGatt gatt, int status) {
            System.out.println("Service discovered");
            //Now we can start reading/writing characteristics
//            readNextSensor(gatt);
            setNotifyNextSensor(gatt);

        }

        @Override
        public void onCharacteristicWrite(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic, int status) {
            System.out.println("on characteristic write");


            //After writing the enable flag, next we read the initial value
            readNextSensor(gatt);
        }

    };

    private static Handler mHandler = new Handler();
    private static boolean scanTrue = true;

    public static void /*String[]*/ scanLeDevices() {
        System.out.println("Before handler");
        scanTrue = true;
        mHandler.postDelayed(new Runnable() {
            @Override
            public void run() {
                System.out.println("Stopping scan");
                m_instance.bluetoothAdapter.stopLeScan(m_instance.mLeScanCallback);
//                scanTrue = false;
                sendList = processScanResult(devicesFound);
                MyJavaNatives.sendScanResult(sendList);
            }
        }, 10000);
        System.out.println("Start scanning");
        uuids[0] = HEART_RATE_SERVICE;
        m_instance.bluetoothAdapter.startLeScan(m_instance.mLeScanCallback);
    }

    private static String[] processScanResult(String[] foundDevices) {
        String[] tempArray;
        if(nrOfDevices > 0){
            tempArray = new String[nrOfDevices];
            for(int i=0;i<nrOfDevices;i++) {
                tempArray[i] = foundDevices[i];
            }
        } else {
            tempArray = new String[1];
            tempArray[0] = "No devices";
        }
        nrOfDevices = 0;
        deviceNumber = 0;
        return tempArray;
    }


    private BluetoothAdapter.LeScanCallback mLeScanCallback = new BluetoothAdapter.LeScanCallback() {

        @Override
        public void onLeScan(final BluetoothDevice device, int rssi, byte[] scanRecord) {
            int length = 0;
            StringBuilder sb = new StringBuilder();
                    for(byte b : scanRecord) {
                        length++;
                        sb.append(String.format("%02X ", b));
                    }

                    if(device.getName() == "null" || device.getName() == "") {
                    } else {
                        devicesFound[deviceNumber] = device.getName();
                        deviceNumber++;
                        nrOfDevices++;
                    }

            System.out.println("Device name: " + device.getName());
            System.out.println("Device address: " + device.getAddress());
            System.out.println("Rssi: " + rssi);
            System.out.println("Length: " + length);
            System.out.println("String: " + sb.toString());
            System.out.println("Scan record: " + scanRecord);
        }
    };

}
