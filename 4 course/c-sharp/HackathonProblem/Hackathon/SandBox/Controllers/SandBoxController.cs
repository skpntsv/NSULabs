using Hackathon.Core.Api.Messages;
using Hackathon.Core.Model.DataBase;
using MassTransit;
using Microsoft.AspNetCore.Mvc;
using SandBox.Services;

namespace SandBox.Controllers;

[ApiController]
[Route("api/sandbox")]
public class SandBoxController(
    ILogger<SandBoxController> logger,
    ExperimentSaveService saveService,
    ExperimentFetchService fetchService,
    IPublishEndpoint publishEndpoint) : ControllerBase
{
    private const int DefaultHackathonRepeats = 10;

    [HttpPost("create")]
    public async Task<IActionResult> CreateExperiment()
    {
        var experiment = await saveService.CreateExperimentAsync(DefaultHackathonRepeats);

        await publishEndpoint.Publish<StartingHackathon>(new
        {
            ExperimentId = experiment.Id,
            HackathonNumber = 1
        });

        logger.LogInformation("Событие запуска хакатона опубликовано.");

        return Ok(new
        {
            Message = "Эксперимент успешно создан.",
            ExperimentId = experiment.Id,
            HackathonRepeats = experiment.HackathonCount
        });
    }

    [HttpGet("{experimentId}/details")]
    public async Task<IActionResult> GetExperimentDetails(int experimentId)
    {
        var experiment = await fetchService.GetExperimentByIdAsync(experimentId);

        if (experiment == null)
        {
            return NotFound("Эксперимент не существует.");
        }

        var statusMessage = experiment.Status switch
        {
            ExperimentStatus.Completed => "Эксперимент завершён.",
            ExperimentStatus.Opened => "Эксперимент открыт и готов к запуску.",
            ExperimentStatus.InProgress => "Эксперимент в процессе выполнения.",
            ExperimentStatus.Failed => "Эксперимент завершился с ошибкой.",
            _ => "Неизвестный статус эксперимента."
        };
        return Ok(new
        {
            experiment.Id,
            experiment.HackathonCount,
            Status = experiment.Status.ToString(),
            StatusMessage = statusMessage
        });
    }

    [HttpGet("{experimentId}/average-score")]
    public async Task<IActionResult> GetAverageScore(int experimentId)
    {
        var averageScore = await fetchService.CalculateAverageScoreAsync(experimentId);

        if (averageScore == null)
        {
            return NotFound("Нет данных о score для данного эксперимента.");
        }

        return Ok(new
        {
            ExperimentId = experimentId, 
            AverageScore = averageScore
        });
    }

    [HttpGet("{experimentId}/scores")]
    public async Task<IActionResult> GetExperimentScores(int experimentId)
    {
        var scores = await fetchService.GetScoresAsync(experimentId);
        if (scores.Count == 0)
        {
            return NotFound("Нет данных о score для данного эксперимента.");
        }
        return Ok(scores);
    }

    [HttpGet("{experimentId}/teams")]
    public async Task<IActionResult> GetExperimentTeams(int experimentId)
    {
        var teams = await fetchService.GetTeamsAsync(experimentId);
        if (teams.Count == 0)
        {
            return NotFound("Нет данных о командах для данного эксперимента.");
        }
        return Ok(teams);
    }
}