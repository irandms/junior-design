package computer.another.just.acswitchcontroller;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;

import computer.another.just.acswitchcontroller.ble.BleManager;

public class StateGetter extends Service {

    private BleManager manager;
    private Thread t;
    private boolean go;
    MainActivity ac;

    public StateGetter(MainActivity m, BleManager bl) {
        manager = bl;
        ac = m;
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        go = true;


        //t = new Thread(r);
        //t.start();
        return Service.START_NOT_STICKY;
    }



    @Override
    public void onDestroy() {
        super.onDestroy();
    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }
}
