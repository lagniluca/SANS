/*
 *  Class generated if the last sans' run was condensed.
 */
public class Condensed_Signal extends Signal
{
    //Condensed slice
    private String slice;

    //Constructor
    public Condensed_Signal() {}
    public Condensed_Signal(String name, String slice)
    {
        super(name);
        this.slice = slice;
    }

    //Getters & Setters
    public void setSlice(String slice) { this.slice = slice; }

    @Override
    protected String sigtostr()
    {
        return new String("Signal:\t" + super.getName() + "\t:\t" + slice);
    }
}