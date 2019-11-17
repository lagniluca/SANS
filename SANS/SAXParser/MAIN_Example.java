import java.util.ArrayList;

/**
 *  Example of a main class. Here we use an absolute path but it's your choice as a developer. Also the implementation can vary,
 *  this is just an example to understand how to use the interface of the parser.
 */

public class Main
{
    static final String filename = "/home/lorenzo/yosys/finalized/OUTPUT/sans.xml";

    public static void main( String[] args )
    {
        Condensed_Parser p = new Condensed_Parser(filename);
        try
        {
            p.parse();
        }
        catch(Exception e)
        {
            e.printStackTrace();
        }
        ArrayList<Module> mod = p.getModules();

        String tmp = "";

        for( Module mod_ : mod )
        {
            tmp += mod_.modtostr();
        }

        System.out.println(tmp);
    }
}