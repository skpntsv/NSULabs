using Hackathon.Core.Api.Messages;
using Hackathon.Core.Api.Records;
using Hackathon.Core.Model.DataBase;
using Hackathon.Core.Model.Mappers;
using HrManagerService.Models.Data;
using Microsoft.EntityFrameworkCore;

namespace HrManagerService.Services;

public class HrManagerService(ApplicationDbContext dbContext, ILogger<HrManagerService> logger)
{
    public async Task SaveNewWishlist(GeneratingWishlist message)
    {
        var wishlistDb = message.Wishlist.ToWishlistDB(message.ExperimentId, message.HackathonCount);
        await dbContext.Wishlists.AddRangeAsync(wishlistDb);
        await dbContext.SaveChangesAsync();
    }

    public async Task DeleteWishListsAndEmployees(GeneratingWishlist message)
    {
        var experimentId = message.ExperimentId;
        var hackathonCount = message.HackathonCount;
        
        var wishlistsToDelete = await dbContext.Wishlists
            .Where(w => w.ExperimentId == experimentId && w.HackathonCount == hackathonCount)
            .ToListAsync();
        var employeesToDelete = await dbContext.Employees
            .Where(e => e.ExperimentId == experimentId && e.HackathonCount == hackathonCount)
            .ToListAsync();
        logger.LogInformation($"Delete {wishlistsToDelete.Count} Wishlists");
        
        dbContext.Wishlists.RemoveRange(wishlistsToDelete);
        dbContext.Employees.RemoveRange(employeesToDelete);
        
        await dbContext.SaveChangesAsync();
    }

    public async Task<int> GetEmployeeNumber(GeneratingWishlist message, EmployeeTypes employeeType)
    {
        var experimentId = message.ExperimentId;
        var hackathonCount = message.HackathonCount;
        
        var employeesNumber = await dbContext.Wishlists
            .CountAsync(w => w.ExperimentId == experimentId && 
                             w.HackathonCount == hackathonCount && 
                             w.Employee.Employee.Type == employeeType);
        return employeesNumber;
    }

    public async Task<List<WishlistDB>> GetWishlistsWithEmployees(GeneratingWishlist message)
    {
        var experimentId = message.ExperimentId;
        var hackathonCount = message.HackathonCount;
        
        return await dbContext.Wishlists
            .Include(w => w.Employee)
            .Include(w => w.PreferredEmployees)
            .Where(w => w.ExperimentId == experimentId && w.HackathonCount == hackathonCount)
            .ToListAsync();
    }
}