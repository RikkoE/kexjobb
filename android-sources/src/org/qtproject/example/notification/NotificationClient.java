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

    private static final int REQUEST_ENABLE_BT = 1;
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
    private static UUID[] uuids = new UUID[1];
    private static String[] servicesList;

    private BluetoothAdapter bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();

    private static String[] devicesFound = new String[20];
    private static String[] devicesFoundAddresses = new String[20];
    private static String[] sendList;
    private static boolean doneConnecting = false;
    private static boolean notReadyToSend = false;
    private static boolean ecgNotifyDisabled = true;
    private static boolean waitForData = true;

    static int deviceNumber = 0;
    static int nrOfDevices = 0;

    private static boolean searching = true;

    private static final String TAG = "Notification client";

    public NotificationClient()
    {
        m_instance = this;
    }

    public static void btON() {
        System.out.println(m_instance.bluetoothAdapter.isEnabled());
        if (!m_instance.bluetoothAdapter.isEnabled()) {
            System.out.println("Turning bluetooth on");
            m_instance.startActivityForResult(new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE), 0);
//            m_instance.bluetoothAdapter.enable();
        }
    }

    public static void btOFF() {
        System.out.println("Turning bluetooth off");
        m_instance.bluetoothAdapter.disable();
    }

    public static void disconnectDevice() {
        m_instance.mBluetoothGatt.disconnect();
        m_instance.mBluetoothGatt = null;
        m_instance.knownServices.clear();
    }

    public static void connectDevice(String deviceName) {
        int i;
        doneConnecting = false;
        String deviceAddress = "";
        for(i = 0; i < sendList.length; i++){
            if(devicesFound[i].equals(deviceName)) {
                System.out.println("Found device!");
                deviceAddress = devicesFoundAddresses[i];
                break;
            }
        }

        System.out.println("Connecting to " + deviceName + " with address " + deviceAddress);

        final BluetoothDevice device = m_instance.bluetoothAdapter.getRemoteDevice(deviceAddress);
        m_instance.mBluetoothGatt = device.connectGatt(m_instance, false, m_instance.mGattCallback);
    }

    public static int updateHeartRate() {
//        String fromNative = natives.print();
        return heartRateVal;
    }

    public static String[] updateServices() {
        while(!doneConnecting);
        return servicesList;
    }

    public static int returnBatteryLevel() {
        return glob_batteryLevel;
    }

    public static void getCharacData(int characChosen) {
        if(knownServices.get(characChosen).equals("Heart rate measurement")) {
            handleServiceConnection(0);
        } else if(knownServices.get(characChosen).equals("Battery level indicator")) {
            handleServiceConnection(1);
        } else if(knownServices.get(characChosen).equals("Manufacturer name")) {
            handleServiceConnection(2);
        } else if(knownServices.get(characChosen).equals("ECG measurement")) {
            handleServiceConnection(3);
        } else {
            System.out.println("Characteristic not found");
        }
    }

    public static void handleServiceConnection(int service) {
        switch(service) {
            case 0:
                System.out.println("Heart");
                BluetoothGattCharacteristic heartCharac = m_instance.mBluetoothGatt.getService(HEART_RATE_SERVICE).getCharacteristic(HEART_RATE_MEASUREMENT);
                m_instance.setNotify(heartCharac);
                break;
            case 1:
                System.out.println("Battery");
                BluetoothGattCharacteristic batteryCharac = m_instance.mBluetoothGatt.getService(BATTERY_LEVEL_SERVICE).getCharacteristic(BATTERY_LEVEL_INDICATOR);
                m_instance.readCharacteristic(batteryCharac);
                break;
            case 2:
                System.out.println("Manufacturer name");
                BluetoothGattCharacteristic manufacCharac = m_instance.mBluetoothGatt.getService(DEVICE_INFORMATION_SERVICE).getCharacteristic(DEVICE_NAME_STRING);
                m_instance.readCharacteristic(manufacCharac);
                break;
            case 3:
                System.out.println("ECG");
                BluetoothGattCharacteristic ecgMeasure = m_instance.mBluetoothGatt.getService(ECG_SERVICE).getCharacteristic(ECG_MEASUREMENT_CHARACTERISTIC);
                m_instance.setNotify(ecgMeasure);
                break;
            default:
                System.out.println("Invalid service");
                break;
        }
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

    public static String[] getData(int characteristicWanted) {
        if(knownServices.get(characteristicWanted).equals("Battery level indicator")) {
            getCharacData(characteristicWanted);
            String batteryLevel[] = {Integer.toString(glob_batteryLevel)};
            return batteryLevel;
        } else if(knownServices.get(characteristicWanted).equals("Manufacturer name")) {
            String manufacName[] = {glob_manufacturerName};
            return manufacName;
        } else if(knownServices.get(characteristicWanted).equals("ECG measurement")) {
            String ecgData[] = new String[7];
            ecgData[0] = Integer.toString(glob_ecgTimeStamp);
//            System.out.println("ECG DATA " + Arrays.toString(ecgData));
            for(int i = 1; i < 7; i++) {
//                System.out.println("ECG DATA " + Arrays.toString(ecgData));
                ecgData[i] = Integer.toString(glob_ecgDataArray[i-1]);
            }
            return ecgData;
        } else {
            String defaultSend[] = {"Data not available"};
            System.out.println("Data not available");
            return defaultSend;
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
        System.out.println("Setting notify on heart rate");
                //Enable local notifications
        mBluetoothGatt.setCharacteristicNotification(characteristic, true);
                //Enabled remote notifications
        BluetoothGattDescriptor desc = characteristic.getDescriptor(CLIENT_CHARACTERISTIC_CONFIG);
        desc.setValue(BluetoothGattDescriptor.ENABLE_NOTIFICATION_VALUE);
        mBluetoothGatt.writeDescriptor(desc);
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

        private void readNextSensor(BluetoothGattCharacteristic characteristic) {
            System.out.println("Read next sensor");
            System.out.println("read charac: " + mBluetoothGatt.readCharacteristic(characteristic));
        }

        @Override
        public void onCharacteristicChanged(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic) {
//            System.out.println("Characteristic changed" + characteristic);
            if(HEART_RATE_MEASUREMENT.equals(characteristic.getUuid())) {
                heartRateVal = characteristic.getIntValue(BluetoothGattCharacteristic.FORMAT_UINT8, 1);
//                MyJavaNatives.sendHeartRate(heartRateVal);
            } else if(ECG_MEASUREMENT_CHARACTERISTIC.equals(characteristic.getUuid())) {
                byte[] bytes;
                bytes = characteristic.getValue();

                glob_ecgDataArray[0] = getIntSixteen(bytes,0);

                int offset = 2;
                for(int i = 1; i < glob_ecgDataArray.length; i++) {
                    glob_ecgDataArray[i] = getIntTwentyFour(bytes, offset);
                    offset += 3;
                }
                waitForData = false;
            } else {
                System.out.println("Characteristic not found");
            }

        }

        @Override
        public void onServicesDiscovered(BluetoothGatt gatt, int status) {
            System.out.println("Service discovered");
            //Now we can start reading/writing characteristics
            servicesList = getGattServices(gatt.getServices());
            doneConnecting = true;
//            setNotifyNextSensor(gatt);
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

    private static Handler mHandler = new Handler();
    private static boolean scanning = true;

    public static void /*String[]*/ scanLeDevices() {
        if(m_instance.bluetoothAdapter.isEnabled()) {
            sendList = null;
            scanning = true;
            mHandler.postDelayed(new Runnable() {
                @Override
                public void run() {
                    System.out.println("Stopping scan");
                    m_instance.bluetoothAdapter.stopLeScan(m_instance.mLeScanCallback);
                    scanning = false;
                    nrOfDevices = 0;
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
        sendList = processScanResult(devicesFound);
        System.out.println("Send list: " + Arrays.toString(sendList));
        System.out.println("Enabled status: " + m_instance.bluetoothAdapter.isEnabled());
        return sendList;
    }

    public static boolean scanningStatus() {
        return scanning;
    }

    private static String[] processScanResult(String[] foundDevices) {
        String[] tempArray;
        if(nrOfDevices > 0){
            tempArray = new String[nrOfDevices];
            for(int i=0;i<nrOfDevices;i++) {
                tempArray[i] = foundDevices[i];
            }
        } else {
            tempArray = new String[0];
//            tempArray[0] = "No devices";
        }
        System.out.println("Devices list: " + Arrays.toString(foundDevices));
//        nrOfDevices = 0;
        deviceNumber = 0; //CHECK THIS LINE LATER FOR ERROR WHEN SCANNING
        return tempArray;
    }

    private BluetoothAdapter.LeScanCallback mLeScanCallback = new BluetoothAdapter.LeScanCallback() {

        @Override
        public void onLeScan(final BluetoothDevice device, int rssi, byte[] scanRecord) {
            int length = 0;
//            StringBuilder sb = new StringBuilder();
//                    for(byte b : scanRecord) {
//                        length++;
//                        sb.append(String.format("%02X ", b));
//                    }

            devicesFound[nrOfDevices] = device.getName();
            devicesFoundAddresses[nrOfDevices] = device.getAddress();
            deviceNumber++;
            nrOfDevices++;

            System.out.println("Device name: " + device.getName());
            System.out.println("Device address: " + device.getAddress());
            System.out.println("Rssi: " + rssi);
//            System.out.println("Length: " + length);
//            System.out.println("String: " + sb.toString());
//            System.out.println("Scan record: " + scanRecord);
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

    private static List<String> knownServices = new ArrayList<String>();

    private String[] getGattServices(List<BluetoothGattService> gattServices) {
        System.out.println(gattServices);
        String uuid = null;
        List<String> servicesAndCharacteristics = new ArrayList<String>();

        for (BluetoothGattService gattService : gattServices) {
            uuid = gattService.getUuid().toString();
//            servicesAndCharacteristics.add("S");
            servicesAndCharacteristics.add(uuid);
            System.out.println("Service uuid: " + uuid);

//            String result = getServiceName(uuid);
//            if(!result.equals("null")) {
//                System.out.println("known service found: " + result);
//                knownServices.add(result);
//            }


            List<BluetoothGattCharacteristic> gattCharacteristics = gattService.getCharacteristics();
            for(BluetoothGattCharacteristic gattCharac : gattCharacteristics) {
                uuid = gattCharac.getUuid().toString();
//                servicesAndCharacteristics.add("C");
                servicesAndCharacteristics.add(uuid);
                System.out.println("\t" + "Characteristic uuid: " + uuid);
                String result = getServiceName(uuid);
                if(!result.equals("null")) {
                    System.out.println("known characteristic found: " + result);
                    m_instance.knownServices.add(result);
                }
            }
        }

        System.out.println("List: " + knownServices);

//        String[] sendServices = new String[servicesAndCharacteristics.size()];
//        sendServices = servicesAndCharacteristics.toArray(sendServices);

        String[] sendKnown = new String[knownServices.size()];
        sendKnown = knownServices.toArray(sendKnown);


        return sendKnown;
    }

}
