package computer.another.just.acswitchcontroller;
import android.bluetooth.BluetoothDevice;

import java.util.ArrayList;
import java.util.UUID;

public class BluetoothDeviceData {
    BluetoothDevice device;
    public int rssi;
    byte[] scanRecord;
    public String advertisedName;           // Advertised name
    private String cachedNiceName;
    private String cachedName;

    // Decoded scan record (update R.array.scan_devicetypes if this list is modified)
    static final int kType_Unknown = 0;
    static final int kType_Uart = 1;
    static final int kType_Beacon = 2;
    static final int kType_UriBeacon = 3;

    public int type;
    int txPower;
    ArrayList<UUID> uuids;

    String getName() {
        if (cachedName == null) {
            cachedName = device.getName();
            if (cachedName == null) {
                cachedName = advertisedName;      // Try to get a name (but it seems that if device.getName() is null, this is also null)
            }
        }

        return cachedName;
    }

    String getNiceName() {
        if (cachedNiceName == null) {
            cachedNiceName = getName();
            if (cachedNiceName == null) {
                cachedNiceName = device.getAddress();
            }
        }

        return cachedNiceName;
    }
}