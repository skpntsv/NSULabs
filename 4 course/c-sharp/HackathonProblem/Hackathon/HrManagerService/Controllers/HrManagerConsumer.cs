using HrManager.Models.Logic;
using HrManagerService.Models.Data;
using MassTransit;
using Microsoft.EntityFrameworkCore;
using Shared.Model.Mappers;
using Shared.Model.Messages;
using Shared.Model.Records;

namespace HrManagerService.Controllers;

public class HrManagerConsumer(ILogger<HrManagerConsumer> _logger,  ApplicationDbContext _dbContext, HrManagerLogic _hrManager) : IConsumer<GeneratingWishlist>
{ 
    private const int RequiredNumber = 5;
    private static readonly SemaphoreSlim _lock = new SemaphoreSlim(1, 1);
    public async Task Consume(ConsumeContext<GeneratingWishlist> context)
    {
        _logger.LogInformation("HrManager received GeneratingWishlist message");
        _logger.LogInformation($"ExperimentId = {context.Message.ExperimentId} | HackathonCount = {context.Message.HackathonCount} | Wishlist = {context.Message.Wishlist}");
        
        var message = context.Message;
        var experimentId = message.ExperimentId;
        var hackathonCount = message.HackathonCount;

        await SaveNewWishlist(_dbContext, message);

        // Считаем количество TeamLead и Junior в базе для данного Hackathon
        var teamLeadsCount = await GetEmployeeNumber(_dbContext, message, EmployeeTypes.TeamLead);
        var juniorsCount = await GetEmployeeNumber(_dbContext, message, EmployeeTypes.Junior);
        
        // Если набрали нужное кол-во, тогда отправляем сообщение и удаляем старое
        
        // PROBLEM: Если у нас 10 челусов одновременно воходят в условие, то как можно сделать так, чтобы
        // 1. Все события должны обрабатываться сразу, как только могут быть обработаны
        // 2. Если хакатоны проводятся параллельно, то все кроме одного не сидят на лочке, ожидая одного
        // 3. Отправляем ровно 1 сообщение и удаляем ровно один раз
        
        bool isWishlistsEnough = (teamLeadsCount == RequiredNumber && juniorsCount == RequiredNumber);
        _logger.LogInformation($"ExperimentId = {context.Message.ExperimentId} | HackathonCount = {context.Message.HackathonCount} | teamLeadCount = {teamLeadsCount} | JuniorCount = {juniorsCount}");
        if (isWishlistsEnough)
        {
            await _lock.WaitAsync();
            try
            {
                teamLeadsCount = await GetEmployeeNumber(_dbContext, message, EmployeeTypes.TeamLead);
                juniorsCount = await GetEmployeeNumber(_dbContext, message, EmployeeTypes.Junior);
                isWishlistsEnough = (teamLeadsCount == RequiredNumber && juniorsCount == RequiredNumber);
                if (isWishlistsEnough) {
                    await PublishGeneratingTeam(_dbContext, context, message);
                    await DeleteWishListsAndEmployees(_dbContext, message);
                }
            }
            finally
            {
                _lock.Release();
            }
        }
        
    }

    private async Task SaveNewWishlist(ApplicationDbContext dbContext, GeneratingWishlist message)
    {
        var wishlistDb = message.Wishlist.ToWishlistDB(message.ExperimentId, message.HackathonCount);
        await dbContext.Wishlists.AddRangeAsync(wishlistDb);
        await dbContext.SaveChangesAsync();
    }

    private async Task PublishGeneratingTeam(ApplicationDbContext dbContext, IPublishEndpoint publisher, GeneratingWishlist message)
    {
        var experimentId = message.ExperimentId;
        var hackathonCount = message.HackathonCount;
        
        var employeesWishlists = await dbContext.Wishlists
            .Include(w => w.Employee)
            .Include(w => w.PreferredEmployees)
            .Where(w => w.ExperimentId == experimentId && w.HackathonCount == hackathonCount)
            .ToListAsync();
        
        var teamLeadsWishlists = employeesWishlists
            .Where(w => w.Employee.Employee.Type == EmployeeTypes.TeamLead)
            .Select(w => w.ToWishlist())
            .ToList();
        
        var juniorsWishLists = employeesWishlists
            .Where(w => w.Employee.Employee.Type == EmployeeTypes.Junior)
            .Select(w => w.ToWishlist())
            .ToList();
        
            await publisher.Publish<GeneratingTeam>(new
            {
                ExperimentId = experimentId,
                HackathonCount = hackathonCount,
                Teams = _hrManager.BuildTeams(teamLeadsWishlists, juniorsWishLists),
            });
    }
    
    private async Task DeleteWishListsAndEmployees(ApplicationDbContext dbContext, GeneratingWishlist message)
    {
        var experimentId = message.ExperimentId;
        var hackathonCount = message.HackathonCount;
        
        var wishlistsToDelete = await dbContext.Wishlists
            .Where(w => w.ExperimentId == experimentId && w.HackathonCount == hackathonCount)
            .ToListAsync();
        var employeesToDelete = await dbContext.Employees
            .Where(e => e.ExperimentId == experimentId && e.HackathonCount == hackathonCount)
            .ToListAsync();
        _logger.LogInformation($"Delete {wishlistsToDelete.Count} Wishlists");
        
        dbContext.Wishlists.RemoveRange(wishlistsToDelete);
        dbContext.Employees.RemoveRange(employeesToDelete);
        
        await dbContext.SaveChangesAsync();
    }

    private async Task<int> GetEmployeeNumber(ApplicationDbContext dbContext, GeneratingWishlist message,
        EmployeeTypes employeeType)
    {
        var experimentId = message.ExperimentId;
        var hackathonCount = message.HackathonCount;
        
        var employeesNumber = await dbContext.Wishlists
            .CountAsync(w => w.ExperimentId == experimentId && 
                                       w.HackathonCount == hackathonCount && 
                                       w.Employee.Employee.Type == employeeType);
        return employeesNumber;
    }
}