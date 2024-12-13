using MassTransit;
using HrManager.Models.Logic;
using HrManagerService.Controllers;
using HrManagerService.Models.Data;
using Microsoft.EntityFrameworkCore;

var builder = WebApplication.CreateBuilder(args);

builder.Services.AddDbContext<ApplicationDbContext>(options =>
{
    // Установка строки подключения к базе данных
    var connectionString = builder.Configuration.GetConnectionString("DefaultConnection");
    options.UseNpgsql(connectionString);
});

builder.Services.AddControllers();

builder.Services.AddMassTransit(x =>
{
    x.AddConsumer<HrManagerConsumer>();
    x.UsingRabbitMq((context, cfg) =>
    {
        
        var rabbitMqUser = builder.Configuration["RabbitMQ:Username"];
        var rabbitMqPass = builder.Configuration["RabbitMQ:Password"];
        var rabbitMqHost = builder.Configuration["RabbitMQ:Host"];
        cfg.Host(rabbitMqHost, "/", h =>
        {
            h.Username(rabbitMqUser);
            h.Password(rabbitMqPass);
        });
        
        cfg.ConfigureEndpoints(context);
    });
});

builder.Services
    .AddTransient<HrManagerLogic>()
    .AddTransient<ITeamBuildingStrategy, SimpleStrategy>();

// Создание приложения
var app = builder.Build();

using (var scope = app.Services.CreateScope())
{
    var dbContext = scope.ServiceProvider.GetRequiredService<ApplicationDbContext>();
    dbContext.Database.EnsureCreated(); // Создает новую базу
}

// Настройка маршрутов
app.MapControllers();

// Запуск приложения
app.Run();