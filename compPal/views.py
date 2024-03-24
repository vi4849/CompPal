from django.http import JsonResponse
from django.shortcuts import render
from django.views.decorators.http import require_http_methods
from .forms import SurveyForm  # Assuming you have a form class for validation

@require_http_methods(["POST"])
def submit_survey(request):
    form = SurveyForm(request.POST)
    if form.is_valid():
        memory_size = form.cleaned_data['memorySize']
        processor = form.cleaned_data['processor']
        #TODO
        # Example: Perform some logic based on selected options
        recommendation = make_recommendation(memory_size, processor)

        response_data = {
            'success': True,
            'message': 'Thank you for your submission!',
            'recommendation': recommendation,  # Include additional data based on form input
        }
        return JsonResponse(response_data)
    else:
        return JsonResponse({'success': False, 'errors': form.errors}, status=400)
    


@require_http_methods(["GET"])
def demo(request):
    return render(request, "base.html")