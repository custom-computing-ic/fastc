//Xinyu Niu, 2012, Max Template

package MyApp_pkg;

import com.maxeler.maxcompiler.v1.managers.MAX3BoardModel;
import com.maxeler.maxcompiler.v1.managers.custom.CustomManager.Target;
import com.maxeler.maxcompiler.v1.managers.BuildConfig;

public class HWBuilder {
  public static void main(String[] args) {
    // create manager with board Model and ID
    AppManager m = new AppManager(MAX3BoardModel.MAX3424A, "HWBuilder", 
        Target.MAXFILE_FOR_HARDWARE);
    // create build configuration
    BuildConfig c = new BuildConfig(BuildConfig.Level.FULL_BUILD);

    // configure how the FPGA design is built
    c.setBuildEffort(BuildConfig.Effort.HIGH);  // LOW,MEDIUM,HIGH,VERY_HIGH
    c.setEnableTimingAnalysis(true);            // generate timing report
    c.setMPPRCostTableSearchRange(1, 4);        // set to enable MPPR
    c.setMPPRParallelism(4);                    // use 4 CPU threads

    // build it!
    m.setBuildConfig(c);
    m.build();
  }
}
