package cookbook.gles;

import android.app.Activity;
import android.os.Bundle;

public class GLESActivity extends Activity {

    GLESView mView;

    @Override protected void onCreate(Bundle icicle) {
        super.onCreate(icicle);
        mView = new GLESView(getApplication());
        setContentView(mView);
        String externalFilesDirPath = getExternalFilesDir(null).getAbsolutePath() + "/files";
        //Log.i("TAG", "dlgmlals3 path : " + externalFilesDirPath);
    }

    @Override protected void onPause() {
        super.onPause();
        mView.onPause();
    }

    @Override protected void onResume() {
        super.onResume();
        mView.onResume();
    }
}
