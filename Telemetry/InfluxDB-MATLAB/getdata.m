function [] = getdata()


%% ------ previous bucketquery code

base_url = 'http://3.134.2.166:8086';
token = 'cBzX7hK34AHjfGCElheFna0jT39u5j_Ebi4vRYWDZP1E-LjAffa7hj85pavKbCp71_6nzpCBQxje-YeGRf4UfQ==';

endpoint = '/api/v2/buckets';

options = weboptions('RequestMethod', 'GET', ...
                     'MediaType', 'application/json', ...
                     'HeaderFields', {'Authorization', ['Token ' token]});

    response = webread([base_url endpoint], options);

for i = 1:length(response.buckets)
    disp(['Bucket Name: ' response.buckets{i}.name ' | Bucket ID: ' response.buckets{i}.id]);
end

%% ------

prompt1 = "Please input the *name* of the bucket you want to query: ";
bucketname = strtrim(input(prompt1,'s'));

dataf = 'from(bucket: "';
dataf2 = '") |> range(start: -';
prompt2 = "Please input the time range of data you want to query, for example, 7d: ";
timerange = input(prompt2,'s');
datab = ')';
data = [dataf bucketname dataf2 timerange datab];

datasaved = data;

body = matlab.net.http.MessageBody(data);
contentTypeField = matlab.net.http.field.ContentTypeField('application/vnd.flux'); 
acceptField = matlab.net.http.field.AcceptField('application/csv');
method = matlab.net.http.RequestMethod.POST;
auth = matlab.net.http.field.GenericField('Authorization','Token cBzX7hK34AHjfGCElheFna0jT39u5j_Ebi4vRYWDZP1E-LjAffa7hj85pavKbCp71_6nzpCBQxje-YeGRf4UfQ=='); 
header = [auth acceptField contentTypeField]; 
request = matlab.net.http.RequestMessage(method,header,body);

uri = matlab.net.URI('http://3.134.2.166:8086/api/v2/query?org=4a7a666528409094'); 
response = send(request,uri);
structuredata = response.Body.Data;

selectedconditions = {};
selectedtags= {};

%---

tester = structuredata(:,7); % assuming 'x_value' is the 7th column


for j = 2:height(tester)
    if isnan(tester{j,1})
        disp(j);
        structuredata(j,:) = [];
    end
end


for i = 1:width(structuredata)
    % List of unique values re-initialized per looped index
    columnName = structuredata.Properties.VariableNames{i};
    columnData = structuredata.(columnName);

    if iscell(columnData) && iscell(columnData{1})
        columnData = cellfun(@(c) c{1}, columnData, 'UniformOutput', false);
    end

    %disp(class(columnData(1)));
    

    disc = 0;

    for i = 1:width(columnData)
        if isa(columnData(i),"double")
            disc = disc + 1;
        
        elseif isa(columnData(i),"datetime")
            disc = disc + 1;
        end
    end

    if disc
        continue
    end
    
    uniqueColumnValues = unique(columnData);
    disp(['Unique values for column "', columnName, '":']);
    disp(uniqueColumnValues);
    
    % Ask the user to select a value to filter by, or skip by pressing enter.
    prompt = sprintf("Please enter a value to filter the '%s' column, or press enter to skip: ", columnName);
    userCondition = input(prompt,'s');
    
    % Apply the filter if the user entered a condition.
    if ~isempty(userCondition)
        selectedtags = [selectedtags columnName];
        structuredata1 = structuredata(strcmp(structuredata.(columnName), userCondition), :);

        % If no rows remain after filtering
        while ((isempty(structuredata1)) && ~isempty(userCondition))
            disp(['No matching data found for the filter: ', columnName, ' = ', userCondition]);
            promptempty = "Please enter a different value to filter by: ";
            userCondition = input(promptempty,"s"); 

            if iscell(structuredata.(columnName)) 
                structuredata1 = structuredata(strcmp(structuredata.(columnName), userCondition), :);
            else 
                structuredata1 = structuredata(structuredata.(columnName) == str2double(userCondition), :);
            end
        end

        if ~isempty(structuredata1)
            structuredata = structuredata1;
            selectedconditions = [selectedconditions userCondition];
        end
    end



structuredata.x_time = datetime(structuredata.x_time, 'InputFormat', 'yyyy-MM-dd''T''HH:mm:ss.SSSSSSSSS''Z''', 'TimeZone', 'UTC');

timeDifferences = structuredata.x_time - min(structuredata.x_time);

relativeSeconds = seconds(timeDifferences);

structuredata.relative_time_seconds = relativeSeconds;

timestamps = structuredata.relative_time_seconds;
sensorvalues = structuredata.x_value;


%saveddata = structuredata(:,["relative_time_seconds","x_value"]);
saveddata = structuredata;

save('savefile.mat', 'saveddata');

plot(timestamps, sensorvalues, '-s', 'MarkerSize', 10, ...
    'MarkerEdgeColor', 'red', ...
    'MarkerFaceColor', [1 .6 .6]);

% Enable the grid
grid on;

% Set labels for the axes
xlabel('relative time (seconds)');
ylabel('sensor value (units)');

% Adjust the current axes
ax = gca;  % Get handle to the current axes

% Set X and Y grid spacing
ax.YTick = floor(min(sensorvalues)):10:ceil(max(sensorvalues));


end

disp(selectedtags);
disp(selectedconditions);

disp("do you want the data to refresh automatically with pre-existing tag filters?");
autoprompt = "input 1 for 'yes', input 0 for 'no'";
autodisc = input(autoprompt);

if autodisc == 1
    loop() 
else
    while autodisc ~= 0
        promptloop = "Please enter a valid value: ";
        autodisc = input(promptloop);
        if autodisc == 1
            
            loop()
            break
        end
    end
end

%------
%------
%------

function [] = loop()
query1 = "please enter the time interval between each loop";
query2 = "please enter the number of loops desired";
interval = input(query1);

% CONDITIONS FOR INPUTS ------

while (~isa(interval,"double") || interval < 0)
    interval = input(query1);
end

numloops = input(query2);

while (numloops < 0 || (rem(numloops,1) ~=0) || ~isa(numloops,"double"))
        numloops = input(query2);
end

% ------

body1= matlab.net.http.MessageBody(datasaved);
contentTypeField1 = matlab.net.http.field.ContentTypeField('application/vnd.flux'); 
acceptField1 = matlab.net.http.field.AcceptField('application/csv');
method1 = matlab.net.http.RequestMethod.POST;
auth1 = matlab.net.http.field.GenericField('Authorization','Token cBzX7hK34AHjfGCElheFna0jT39u5j_Ebi4vRYWDZP1E-LjAffa7hj85pavKbCp71_6nzpCBQxje-YeGRf4UfQ=='); 
header1 = [auth1 acceptField1 contentTypeField1]; 
request1 = matlab.net.http.RequestMessage(method1,header1,body1);

uri1 = matlab.net.URI('http://3.134.2.166:8086/api/v2/query?org=4a7a666528409094'); 
response1 = send(request1,uri1);
structuredata2 = response1.Body.Data;

counter = 0;

%---

tester1 = structuredata2(:,7); % assuming 'x_value' is the 7th column


for c = 2:height(tester1)
    if isnan(tester1{c,1})
        disp(c);
        structuredata2(c,:) = [];
    end
end


%---

for i = 1:numloops
    for j = 1:width(selectedconditions)
        structuredata2 = structuredata2(strcmp(structuredata2.(selectedtags{j}), selectedconditions{j}), :);
    end
    
    structuredata2.x_time = datetime(structuredata2.x_time, 'InputFormat', 'yyyy-MM-dd''T''HH:mm:ss.SSSSSSSSS''Z''', 'TimeZone', 'UTC');
    timeDifferences1 = structuredata2.x_time - min(structuredata2.x_time);
    relativeSeconds1 = seconds(timeDifferences1);
    structuredata2.relative_time_seconds = relativeSeconds1;
    timestamps1 = structuredata2.relative_time_seconds;
    sensorvalues1 = structuredata2.x_value;
    saveddata1 = structuredata2(:,["relative_time_seconds","x_value"]);
    save('refreshedsavedfile.mat', 'saveddata1');
    plot(timestamps1, sensorvalues1, '-s', 'MarkerSize', 10, ...
    'MarkerEdgeColor', 'blue', ...
    'MarkerFaceColor', [1 .6 .6]);
    grid on;
    xlabel('relative time (seconds)');
    ylabel('sensor value (units)');
    ax = gca;
    ax.YTick = floor(min(sensorvalues1)):10:ceil(max(sensorvalues1));

    counter = counter + 1;

    pause(interval)
end

disp(['refreshed ',num2str(counter),' times.'])

%------
%------
%------

end
end
