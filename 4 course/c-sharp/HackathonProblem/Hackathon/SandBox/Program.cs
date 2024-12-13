using MassTransit;
using Microsoft.EntityFrameworkCore;
using SandBox.Controllers;
using SandBox.Models.Data;

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
    x.AddConsumer<SandBoxConsumer>();
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


// Создание приложения
var app = builder.Build();

// Настройка маршрутов
app.MapControllers();

// Запуск приложения
app.Run();