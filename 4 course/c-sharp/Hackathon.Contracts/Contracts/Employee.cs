namespace Hackathon.Contracts
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
    }
}
