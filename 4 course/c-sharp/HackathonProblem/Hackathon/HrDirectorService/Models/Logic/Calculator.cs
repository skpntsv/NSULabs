namespace HrDirectorService.Models.Logic;

public class Calculator
{
    public static double CalculateHarmonicMean(IEnumerable<int> values) 
    {
        int size = values.Count();
        double sumOfReciprocals = values.Sum(x => 1.0 / x);

        return size / sumOfReciprocals;
    }
}