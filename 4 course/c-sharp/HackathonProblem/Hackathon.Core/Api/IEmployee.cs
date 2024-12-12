using Hackathon.Contracts;

namespace Hackathon.Core.Api;

public interface IEmployee
{
    public int Id { get; }
    public string Name { get; }

    public IWishlist GenerateWishlist();
}