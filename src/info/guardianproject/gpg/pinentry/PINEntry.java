package info.guardianproject.gpg.pinentry;

import android.os.Bundle;
import android.app.Activity;
import android.view.Menu;

public class PINEntry extends Activity {

	ServerSocketThread mServerSocket;
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        if( mServerSocket != null ) {
        	mServerSocket.setStopThread(true);
        	mServerSocket.stop();
        }
        mServerSocket = new ServerSocketThread();
        mServerSocket.start();

        setContentView(R.layout.activity_pinentry);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.activity_pinentry, menu);
        return true;
    }
}
