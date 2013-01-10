package MyApp_pkg;

import com.maxeler.maxcompiler.v1.managers.BuildConfig;
import com.maxeler.maxcompiler.v1.managers.BuildConfig.Level;
import com.maxeler.maxcompiler.v1.managers.MAX3BoardModel;
import com.maxeler.maxcompiler.v1.managers.custom.CustomManager.Target;
import com.maxeler.maxcompiler.v1.managers.BuildConfig.Effort;
import com.maxeler.maxcompiler.v1.managers.custom.stdlib.DebugLevel;


public class HWBuilder {
	public static void main(String argv[]) {
		AppManager m = new AppManager(MAX3BoardModel.MAX3424A, "HWBuilder", Target.MAXFILE_FOR_HARDWARE);
    
//  DebugLevel d = new DebugLevel();
//  d.setHasMemoryControllerDebugRegisters(true);
//  d.setHasMemoryControllerExtraDebugRegisters(true);
//  d.setHasStreamStatus(true);
//  d.setHasStreamStatusChecksums(true);
//  m.debug.setDebugLevel(d);

    BuildConfig a=new BuildConfig(Level.FULL_BUILD);
//  a.setMPPRParallelism(3);
//  a.setMPPRCostTableSearchRange(4,6);
    a.setBuildEffort (Effort.HIGH);

    m.config.setDefaultStreamClockFrequency(100);
    m.config.setOnCardMemoryFrequency(400);
		m.setBuildConfig(a);
		m.build();
	}
}
