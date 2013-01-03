//Xinyu Niu, 2012, Max Template

package MyApp_pkg;

import com.maxeler.maxcompiler.v1.managers.MAX3BoardModel;
import com.maxeler.maxcompiler.v1.managers.custom.CustomManager.Target;

public class SimBuilder {
  public static void main(String[] args) {
    // create manager with board Model and ID
    AppManager m = new AppManager(MAX3BoardModel.MAX3424A, "SimBuilder",
        Target.MAXFILE_FOR_SIMULATION);
    // build it!
    m.build();
  }
}
