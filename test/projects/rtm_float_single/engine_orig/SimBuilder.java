package engine_orig;

import com.maxeler.maxcompiler.v1.managers.BuildConfig;
import com.maxeler.maxcompiler.v1.managers.BuildConfig.Level;
import com.maxeler.maxcompiler.v1.managers.MAX3BoardModel;
import com.maxeler.maxcompiler.v1.managers.custom.CustomManager.Target;
import com.maxeler.maxcompiler.v1.managers.custom.stdlib.DebugLevel;

//import config.BoardModel;

public class SimBuilder {
	public static void main(String argv[]) {
		AppManager m = new AppManager(MAX3BoardModel.MAX3424A, "SimBuilder", Target.MAXFILE_FOR_SIMULATION);
		m.setBuildConfig(new BuildConfig(Level.FULL_BUILD));

    // for debug only
    DebugLevel d = new DebugLevel();
    d.setHasMemoryControllerDebugRegisters(true);
    d.setHasMemoryControllerExtraDebugRegisters(true);
    d.setHasStreamStatus(true);
    d.setHasStreamStatusChecksums(true);
    m.debug.setDebugLevel(d);

		m.build();
	}
}
