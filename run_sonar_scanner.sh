mkdir -p pytest_reports/coverage

pytest -v python_tests --cov=python/rsgislib --cov-report=xml:pytest_reports/coverage.xml --cov-report=html:pytest_reports/coverage/ --junitxml=pytest_reports/junit.xml --html=pytest_reports/rsgisib_tests_report.html --self-contained-html

sonar-scanner
# \
#  -Dsonar.organization=remotesensinginfo \
#  -Dsonar.projectKey=remotesensinginfo_rsgislib \
#  -Dsonar.sources=python/rsgislib \
#  -Dsonar.host.url=https://sonarcloud.io \
#  -Dsonar.python.coverage.reportPaths=pytest_reports/coverage.xml
