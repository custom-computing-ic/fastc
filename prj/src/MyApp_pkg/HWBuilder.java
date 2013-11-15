package MyApp_pkg;

import com.maxeler.maxcompiler.v1.managers.BuildConfig;
import com.maxeler.maxcompiler.v1.managers.BuildConfig.Level;
import com.maxeler.maxcompiler.v1.managers.MAX3BoardModel;
import com.maxeler.maxcompiler.v1.managers.custom.CustomManager.Target;
import com.maxeler.maxcompiler.v1.managers.BuildConfig.Effort;


public class HWBuilder {
	public static void main(String argv[]) {
		AppManager m = new AppManager(MAX3BoardModel.MAX3424A, "HWBuilder", Target.MAXFILE_FOR_HARDWARE);
    
    BuildConfig a=new BuildConfig(Level.FULL_BUILD);

    a.setMPPRParallelism(6);
    a.setMPPRCostTableSearchRange(1,6);
    //a.setBuildEffort(Effort.VERY_HIGH);

    m.config.setDefaultStreamClockFrequency(150);
		m.setBuildConfig(a);
		m.build();
	}
}
