using Hackathon.Core.Api;
using Hackathon.Core.Implementation;
using MassTransit;
using HrManagerService.Consumers;
using HrManagerService.Models.Data;
using Microsoft.EntityFrameworkCore;
using HrManager = HrManagerService.Models.HrManager;

var builder = WebApplication.CreateBuilder(args);

builder.Services.AddDbContext<ApplicationDbContext>(options =>
{
    // Установка строки подключения к базе данных
    var connectionString = builder.Configuration.GetConnectionString("DefaultConnection");
    options.UseNpgsql(connectionString);
});

builder.Services.AddScoped<HrManagerService.Services.HrManagerService>();

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
    .AddTransient<HrManager>()
    .AddTransient<ITeamBuildingStrategy, SmartBuildingStrategy>();

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