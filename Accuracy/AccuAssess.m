function Results  = AccuAssess(Labels,PreLabels)

[y x z] = size(Labels);
pix = y*x;
Labels = reshape(Labels, 1, pix);
PreLabels = reshape(PreLabels, 1, pix);

[ConfusionMatrix, order]=confusionmat(Labels', PreLabels');

NumClases = length(order);

n  = sum(ConfusionMatrix(:));                  
PA = sum(diag(ConfusionMatrix));
OA  = PA/n;

ClassAcc = double(size(2,NumClases));

for i =1: NumClases;
    ClassAcc(1,i)=ConfusionMatrix(i,i) / sum(ConfusionMatrix(i,:));
    ClassAcc(2,i)= ConfusionMatrix(i,i) / sum(ConfusionMatrix(:,i));
end

 % Estimated Overall Cohen's Kappa (suboptimal implementation)
npj = sum(ConfusionMatrix,1);
nip = sum(ConfusionMatrix,2);
PE  = npj*nip;
if (n*PA-PE) == 0 && (n^2-PE) == 0
    % Solve indetermination
    %warning('0 divided by 0')
    Kappa = 1;
else
    Kappa  = (n*PA-PE)/(n^2-PE);
end   

% Cohen's Kappa Variance
theta1 = OA;
theta2 = PE/n^2;
theta3 = (nip'+npj) * diag(ConfusionMatrix)  / n^2;

suma4 = 0;
for i=1:NumClases
    for j=1:NumClases
         suma4 = suma4 + ConfusionMatrix(i,j)*(nip(i) + npj(j))^2;
    end;
end;

theta4 = suma4/n^3;
varKappa = ( theta1*(1-theta1)/(1-theta2)^2 +2*(1-theta1)*(2*theta1*theta2-theta3)/(1-theta2)^3      + (1-theta1)^2*(theta4-4*theta2^2)/(1-theta2)^4  )/n;
Z = Kappa/sqrt(varKappa);
CI = [Kappa + 1.96*sqrt(varKappa), Kappa - 1.96*sqrt(varKappa)];

%Results structur
Results.LabelOrder= order;
Results.ClassAcc = ClassAcc;
Results.ConfusionMatrix = ConfusionMatrix;
Results.Kappa           = Kappa;   
Results.OA              = 100*OA;      
Results.varKappa        = varKappa;
Results.Z               = Z;       
Results.CI              = CI;



