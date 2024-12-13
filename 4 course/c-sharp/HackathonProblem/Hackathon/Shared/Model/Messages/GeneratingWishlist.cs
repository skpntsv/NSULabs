using Shared.Model.Records;

namespace Shared.Model.Messages;

public class GeneratingWishlist {
    public required int ExperimentId { get; set; } 
    public required int HackathonCount {get; set;}
    public required Wishlist Wishlist { get; set; }
        
}