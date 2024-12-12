using System;
using System.Collections.Generic;
using System.Linq;

namespace Hackathon.Core.Models
{
    public record Employee(int Id, string Name)
    {
        public Wishlist GenerateWishlist(IEnumerable<Employee> potentialPartners)
        {
            return GenerateRandomWishlist(potentialPartners);
        }

        private Wishlist GenerateRandomWishlist(IEnumerable<Employee> potentialPartners)
        {
            var random = new Random();
            var shuffledPartners = potentialPartners
                .OrderBy(it => random.Next())
                .Select(it => it.Id)
                .ToArray();

            return new Wishlist(Id, shuffledPartners);
        }

        public void SendWishlistToHrManager(Wishlist wishlist, string type, HrManager hrManager) 
        {
            hrManager.ReceiveWishlist(wishlist, type);
        }

        public override string ToString()
        {
            return $"Employee({Id} : {Name})";
        }
    }
}
