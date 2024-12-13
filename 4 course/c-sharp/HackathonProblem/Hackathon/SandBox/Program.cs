using MassTransit;
using Microsoft.EntityFrameworkCore;
using SandBox.Consumers;
using SandBox.Models.Data;
using SandBox.Services;

var builder = WebApplication.CreateBuilder(args);

// Настройка строки подключения к базе данных
builder.Services.AddDbContext<ApplicationDbContext>(options =>
{
    var connectionString = builder.Configuration.GetConnectionString("DefaultConnection");
    options.UseNpgsql(connectionString);
});

// Регистрация сервисов в DI
builder.Services.AddScoped<ExperimentSaveService>(); // Сервис для сохранения данных
builder.Services.AddScoped<ExperimentFetchService>(); // Сервис для извлечения данных

// Добавление контроллеров
builder.Services.AddControllers();

// Настройка MassTransit
builder.Services.AddMassTransit(x =>
{
    // Регистрация consumer
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