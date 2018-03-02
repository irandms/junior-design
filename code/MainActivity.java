/**
 * Author: Jacob Ficker
 * Filename: MainActivity.java
 * Description: Contains all the codes that makes the main
 *              and only activity in this app work.
 */

package computer.another.just.acswitchcontroller;

import android.Manifest;
import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGattCharacteristic;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.SystemClock;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ToggleButton;


import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.UUID;

import computer.another.just.acswitchcontroller.ble.BleDevicesScanner;
import computer.another.just.acswitchcontroller.ble.BleManager;
import computer.another.just.acswitchcontroller.ble.BleUtils;

/*
* This class inherits from the Communicator class which
* implements BLE Manager and inherits from compatactivity
* A lot of this code is taken directly from adafruit
* (the manufacturer of the bluetooth device). Their
* code is open source. This file and the layout files
* are the only files that contain original code.
*/

public class MainActivity extends Communicator {

    /*
        PACKET FORMAT FOR BT COMMUNICATIONS WITH DEVICE:
        1st byte is a character, A = Channel one, B = Channel two, R = Request Data
        2nd Byte is a character representing the command.
        N = Turn Channel On, F = Turn Channel off, S = Set timer
        3rd/4th byte is two character. It represents the how long the
        timer should be set for. (Requires conversion currently)
     */

    private final static String TAG = MainActivity.class.getSimpleName();

    /*
    Below are a series of variables that are needed
    for the app to function. Many of these will be
    explained in greater detail later.
    */
    private StateGetter sg;
    private Intent intent;
    private BleManager blem; //Bluetooth LE manager
    private ArrayList<BluetoothDeviceData> mScannedDevices; // to keep track of all available devices
    private BleDevicesScanner mScanner; //scans for devices
    private BluetoothDeviceData theDevice; //this will store the actual device
    private final static long kMinDelayToUpdateUI = 200; //
    private long mLastUpdateMillis;
    private UartDataChunk mDataBuffer;
    private volatile int mReceivedBytes;
    private String currentData;

    //UI Stuff
    private ToggleButton c1, c2;
    private Button timer1, timer2, con;
    private EditText time1, time2;
    private TextView tr1, tr2, constat;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        intent = new Intent(this, StateGetter.class);
        sg = new StateGetter(this, blem);

        //Initializing all of the UI variables
        c1 = (ToggleButton) findViewById(R.id.toggleButton); //these two are the toggle buttons
        c2 = (ToggleButton) findViewById(R.id.toggleButton2);
        time1 = (EditText) findViewById(R.id.num1); //number inputs for timers
        time2 = (EditText) findViewById(R.id.num2);
        tr1 = (TextView) findViewById(R.id.tr1); //time remaining text display
        tr2 = (TextView) findViewById(R.id.tr2);
        constat = (TextView) findViewById(R.id.conn); // displays connection status
        timer1 = findViewById(R.id.button); //these are the set timer buttons
        timer2 = findViewById(R.id.button2);
        con = findViewById(R.id.conb); // connect button
        currentData = ""; //stores current received data
        constat.setText("Connection Status:"); // this text changes frequently


        /*
        * Below is a some code that ensures
         * the user has given the app permission
         * to use location services for BLE
        */
        int permissionCheck = ContextCompat.checkSelfPermission(this, Manifest.permission.ACCESS_FINE_LOCATION);

        if (permissionCheck != PackageManager.PERMISSION_GRANTED){

            if (ActivityCompat.shouldShowRequestPermissionRationale(this, Manifest.permission.ACCESS_FINE_LOCATION)){
                Toast.makeText(this, "The permission to get BLE location data is required", Toast.LENGTH_SHORT).show();

            }else{
                requestPermissions(new String[]{Manifest.permission.ACCESS_COARSE_LOCATION, Manifest.permission.ACCESS_FINE_LOCATION}, 1);

            }

        }else{
            Toast.makeText(this, "Location permissions already granted", Toast.LENGTH_SHORT).show();
            //this will show after you have already given permission
        }


        blem = new BleManager(this); // creating a new BLE Manager. Taken from AdaFruit code.
        BluetoothAdapter bluetoothAdapter = BleUtils.getBluetoothAdapter(getApplicationContext()); //create new adapter

        /*
        * This is the way the app finds and stores all of the
        * available bluetooth devices
        */
        mScanner = new BleDevicesScanner(bluetoothAdapter, null, new BluetoothAdapter.LeScanCallback() {
            @Override
            public void onLeScan(final BluetoothDevice device, final int rssi, byte[] scanRecord) { //for scanning for devices

                BluetoothDeviceData previouslyScannedDeviceData = null;

                if (mScannedDevices == null)
                    mScannedDevices = new ArrayList<>();       // Safeguard

                // Check that the device was not previously found
                for (BluetoothDeviceData deviceData : mScannedDevices) {
                    if (deviceData.device.getAddress().equals(device.getAddress())) {
                        previouslyScannedDeviceData = deviceData;
                        break;
                    }
                }

                BluetoothDeviceData deviceData;
                if (previouslyScannedDeviceData == null) {
                    // Add it to the mScannedDevice list
                    deviceData = new BluetoothDeviceData();
                    mScannedDevices.add(deviceData);
                } else {
                    deviceData = previouslyScannedDeviceData;
                }

                //set device data before decoding it
                deviceData.device = device;
                deviceData.rssi = rssi;
                deviceData.scanRecord = scanRecord;
                decodeScanRecords(deviceData);
            }
        });

        blem.setBleListener(this);
        this.mBleManager = blem; //set equal for other functions use

        mScanner.start(); //start service


        //toggle listener for channel one
        c1.setOnCheckedChangeListener(new ToggleButton.OnCheckedChangeListener() {
            public void onCheckedChanged(CompoundButton b, boolean toggled) {
                if (toggled) { //send on bytes
                    sendData("AN".getBytes());
                } else { //send off bytes
                    sendData("AF".getBytes());
                }
            }
        });

        //toggle listener for channel two
        c2.setOnCheckedChangeListener(new ToggleButton.OnCheckedChangeListener() {
            public void onCheckedChanged(CompoundButton b, boolean toggled) {
                if (toggled) { //send on bytes
                    sendData("BN".getBytes());
                } else { //send off bytes
                    sendData("BF".getBytes());
                }
            }
        });

        //button on-click listener
        timer1.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                c1.setChecked(true); //change toggle to on state
                setTimer(0); //send timer data
            }
        });

        //button on-click listener
        timer2.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                c2.setChecked(true); //change toggle to on state
                setTimer(1); //send timer data
            }
        });

        //connect button on-click listener
        con.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                connect(); //call connect function
                Toast.makeText(getApplicationContext(), "Attempting to Connect...", Toast.LENGTH_SHORT).show();
                //inform user
            }
        });


    }


    /*
    * This function clears the UI when the phone
    * connects or disconnects the to/from the device.
    * Informs user of connection status.
    * */
    private void clearUI(int c) {
/*
       if (c == 0){
            c1.setChecked(false); //set toggle off
            c2.setChecked(false);
            time1.setText(""); //remove timer text
            time2.setText("");
            tr1.setText("Time Remaining: --"); //remove time remaining text
            tr2.setText("Time Remaining: --");
            constat.setText("Connection Status: Disconnected"); //inform user of connection status

       } else {
            constat.setText("Connection Status: Connected!"); //inform user of connection status

       }
*/
    }

    //Necessary override method from Communicator
    @Override
    public void onDisconnected() {
        clearUI(0); //remove info about device state
        constat.setText("Connection Status: Disconnected"); //inform user of connection status
    }

    //Necessary override method from Communicator
    @Override
    public void onConnected() {

        clearUI(1); //inform user that we are connected to the device

        //create send R thread
        Runnable r = new Runnable() {
            @Override
            public void run() {

                        //determmines when thread should stop running
                        boolean go = true;
                        while(go) {
                            //wait five seconds
                            long future = System.currentTimeMillis() + 5000;
                            synchronized (this) {
                                try {
                                    wait(future - System.currentTimeMillis());
                                } catch (InterruptedException e) {
                                    e.printStackTrace();
                                    Log.w(TAG, "Fail..."); //log if failed
                                }
                            }

                            //check if connected
                            if(blem.getState() > 0) {
                                //if connected send R to request data from device
                                sendData("R");
                                Log.i(TAG, "Sending R!"); //log activity

                            } else { //if not stop thread
                                go = false;
                            }
                        }
                    }
                };

        //start thread
        Thread t = new Thread(r);
        t.start();

    }

    //Finds UART BLE service
    @Override
    public void onServicesDiscovered() {
        mUartService = blem.getGattService(UUID_SERVICE); //sets variable to uart service
        enableRxNotifications();
    }

    /*
    * Sends timer data to the bluetooth module
    * 0 = channel one, 1 = channel two
    */
    private void setTimer(int channel) {

        //variables for later use
        EditText num;
        String s = "";

        if (channel == 0) {
            num = (EditText) findViewById(R.id.num1); //get the user input field
            s = num.getText().toString(); //get user input
            s = "AS"+s; //set string channel one timer code
        } else {
            num = (EditText) findViewById(R.id.num2); //get the user input field
            s = num.getText().toString(); //get user input
            s = "BS"+s; //set string to channel two timer code
        }
        sendData(s.getBytes()); //send data
    }

    /*
    * This function uses BLE scan data and finds
    * an adafruit module. Upon finding it, it connects
    * automatically using BLE manager blem.
    */
    private void connect() {

        //variables for later
        BluetoothDeviceData dd;
        boolean anyluck = false;

        //search through scan data
        for (int i = 0; i < mScannedDevices.size(); i++) {
                dd = mScannedDevices.get(i);
                if (dd.getNiceName().trim().equals("Adafruit Bluefruit LE")) { //check if correct module
                    theDevice = dd; //save device data
                    anyluck = true; //found device
                    break;
                }
            }

        if(anyluck) { //if device is found

            //get address of device as string
            BluetoothDevice btd = theDevice.device;
            String addr = btd.getAddress();

           if(blem.connect(this, addr)) { //attempt to connect
               constat.setText("Connection Status: Connected!"); //inform user
           } else {
               constat.setText("Connection Status: Failed"); //inform user
           }
        } else {
            constat.setText("Could Not Find Device."); //inform user
        }

    }

    //deprecated
   /* private void updateUI() {

        //sendDataWithCRC("Hello".getBytes());

        String get = "";
        TextView fillme = findViewById(R.id.conn);
        if(mDataBuffer != null) {
            fillme.setText(mDataBuffer.getData().toString());
        } else {
            Log.w(TAG, "Empty Return Data!!!");
        }

    } */



    /*   This code is take directly from Adafruit's app
        It looks like they used someone else's code as well.   */

    private void decodeScanRecords(BluetoothDeviceData deviceData) {
        // based on http://stackoverflow.com/questions/24003777/read-advertisement-packet-in-android
        final byte[] scanRecord = deviceData.scanRecord;

        ArrayList<UUID> uuids = new ArrayList<>();
        byte[] advertisedData = Arrays.copyOf(scanRecord, scanRecord.length);
        int offset = 0;
        deviceData.type = BluetoothDeviceData.kType_Unknown;

        // Check if is an iBeacon ( 0x02, 0x0x1, a flag byte, 0x1A, 0xFF, manufacturer (2bytes), 0x02, 0x15)
        final boolean isBeacon = advertisedData[0] == 0x02 && advertisedData[1] == 0x01 && advertisedData[3] == 0x1A && advertisedData[4] == (byte) 0xFF && advertisedData[7] == 0x02 && advertisedData[8] == 0x15;

        // Check if is an URIBeacon
        final byte[] kUriBeaconPrefix = {0x03, 0x03, (byte) 0xD8, (byte) 0xFE};
        final boolean isUriBeacon = Arrays.equals(Arrays.copyOf(scanRecord, kUriBeaconPrefix.length), kUriBeaconPrefix) && advertisedData[5] == 0x16 && advertisedData[6] == kUriBeaconPrefix[2] && advertisedData[7] == kUriBeaconPrefix[3];

        if (isBeacon) {
            deviceData.type = BluetoothDeviceData.kType_Beacon;

            // Read uuid
            offset = 9;
            UUID uuid = BleUtils.getUuidFromByteArrayBigEndian(Arrays.copyOfRange(scanRecord, offset, offset + 16));
            uuids.add(uuid);
            offset += 16;

            // Skip major minor
            offset += 2 * 2;   // major, minor

            // Read txpower
            final int txPower = advertisedData[offset++];
            deviceData.txPower = txPower;
        } else if (isUriBeacon) {
            deviceData.type = BluetoothDeviceData.kType_UriBeacon;

            // Read txpower
            final int txPower = advertisedData[9];
            deviceData.txPower = txPower;
        } else {
            // Read standard advertising packet
            while (offset < advertisedData.length - 2) {
                // Length
                int len = advertisedData[offset++];
                if (len == 0) break;

                // Type
                int type = advertisedData[offset++];
                if (type == 0) break;

                // Data
//            Log.d(TAG, "record -> lenght: " + length + " type:" + type + " data" + data);

                switch (type) {
                    case 0x02:          // Partial list of 16-bit UUIDs
                    case 0x03: {        // Complete list of 16-bit UUIDs
                        while (len > 1) {
                            int uuid16 = advertisedData[offset++] & 0xFF;
                            uuid16 |= (advertisedData[offset++] << 8);
                            len -= 2;
                            uuids.add(UUID.fromString(String.format("%08x-0000-1000-8000-00805f9b34fb", uuid16)));
                        }
                        break;
                    }

                    case 0x06:          // Partial list of 128-bit UUIDs
                    case 0x07: {        // Complete list of 128-bit UUIDs
                        while (len >= 16) {
                            try {
                                // Wrap the advertised bits and order them.
                                UUID uuid = BleUtils.getUuidFromByteArraLittleEndian(Arrays.copyOfRange(advertisedData, offset, offset + 16));
                                uuids.add(uuid);

                            } catch (IndexOutOfBoundsException e) {
                                Log.e("FRUIT", "BlueToothDeviceFilter.parseUUID: " + e.toString());
                            } finally {
                                // Move the offset to read the next uuid.
                                offset += 16;
                                len -= 16;
                            }
                        }
                        break;
                    }

                    case 0x09: {
                        byte[] nameBytes = new byte[len - 1];
                        for (int i = 0; i < len - 1; i++) {
                            nameBytes[i] = advertisedData[offset++];
                        }

                        String name = null;
                        try {
                            name = new String(nameBytes, "utf-8");
                        } catch (UnsupportedEncodingException e) {
                            e.printStackTrace();
                        }
                        deviceData.advertisedName = name;
                        break;
                    }

                    case 0x0A: {        // TX Power
                        final int txPower = advertisedData[offset++];
                        deviceData.txPower = txPower;
                        break;
                    }

                    default: {
                        offset += (len - 1);
                        break;
                    }
                }
            }

            // Check if Uart is contained in the uuids
            boolean isUart = false;
            for (UUID uuid : uuids) {
                if (uuid.toString().equalsIgnoreCase("6e400001-b5a3-f393-e0a9-e50e24dcca9e")) {
                    isUart = true;
                    break;
                }
            }
            if (isUart) {
                deviceData.type = BluetoothDeviceData.kType_Uart;
            }
        }

        deviceData.uuids = uuids;
    }


    //cleanup
    @Override
    protected void onDestroy() {
        super.onDestroy();
        mScanner.stop();
    }

    /*
    * This code is also take directly from AdaFruit's app
    * I've modified it slightly to suit my needs.
    * */
    @Override
    public synchronized void onDataAvailable(BluetoothGattCharacteristic characteristic) {
        super.onDataAvailable(characteristic);

        // UART RX
        if (characteristic.getService().getUuid().toString().equalsIgnoreCase(UUID_SERVICE)) { //check if uart data is available

            if (characteristic.getUuid().toString().equalsIgnoreCase(UUID_RX)) { //check for receivable data

                //store data bytes
                final byte[] bytes = characteristic.getValue();
                mReceivedBytes += bytes.length;

                //create data chunk
                //the data chunk class is currently not really utilized,
                //but it may need to be used in the future
                final UartDataChunk dataChunk = new UartDataChunk(System.currentTimeMillis(), UartDataChunk.TRANSFERMODE_RX, bytes);
                String fill = new String(dataChunk.getData());

                //store received data
                currentData = fill;

                //display recevied data for testing purposes
                ((TextView) findViewById(R.id.powone)).setText("Channel One: " + fill);
                Log.w(TAG, "Data Processed");


            }
        }
    }

}


